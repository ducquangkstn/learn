package server

import (
	"errors"
	"fmt"
	"io"
	"net"
	"slices"
	"strconv"
	"strings"

	"go.uber.org/zap"
)

type Command struct {
	handle func(size int) error
}
type Handler struct {
	conn net.Conn
	buf  []byte
	tmp  []byte

	commands map[string]Command

	server *Server
}

func NewHandler(conn net.Conn, server *Server) *Handler {
	h := &Handler{
		conn:   conn,
		buf:    make([]byte, 1024),
		server: server,
	}
	h.commands = map[string]Command{
		"ping": {handle: h.ping},
		"echo": {handle: h.echo},
		"get":  {handle: h.get},
		"set":  {handle: h.set},
	}

	return h
}

func (h *Handler) ListenAndServe() {
	defer func() {
		cErr := h.conn.Close()
		if cErr != nil {
			zap.L().Error("failed to close connection", zap.Error(cErr))
		}
	}()
	for {
		err := h.serve()
		switch {
		case errors.Is(err, io.EOF):
			zap.L().Info("connection is closed")
			return
		case err != nil:
			zap.L().Info("failed to handle connection", zap.Error(err))
			return
		default:
		}
	}
}

func (h *Handler) serve() error {
	size, err := h.readInt('*')
	if err != nil {
		return fmt.Errorf("read size: %w", err)
	}
	zap.L().Debug("read command length", zap.Int("size", size))
	if size == 0 {
		return nil
	}

	cmd, err := h.readBulkString()
	if err != nil {
		return fmt.Errorf("read bulk string: %w", err)
	}

	cmd = strings.ToLower(cmd)

	command, ok := h.commands[cmd]
	if !ok {
		return fmt.Errorf("unsupported command: %s", cmd)
	}
	return command.handle(size)
}

func (h *Handler) nextToken() ([]byte, error) {
	for {
		for len(h.tmp) < 2 {
			n, err := h.conn.Read(h.buf)
			if err != nil {
				return nil, fmt.Errorf("read bytes from conn: %w", err)
			}
			h.tmp = append(h.tmp, h.buf[:n]...)
		}
		re := -1
		for i := range len(h.tmp) - 1 {
			if h.tmp[i] == '\r' && h.tmp[i+1] == '\n' {
				re = i
				break
			}
		}
		if re == -1 {
			continue
		}

		out := slices.Clone(h.tmp[:re])
		h.tmp = slices.Clone(h.tmp[re+2:])
		zap.L().Info("next token", zap.String("token", string(out)), zap.String("remaining", string(h.tmp)))
		return out, nil
	}
}

func (h *Handler) readInt(leadingCh byte) (int, error) {
	t1, err := h.nextToken()
	if err != nil {
		return 0, fmt.Errorf("next token: %w", err)
	}

	if t1[0] != leadingCh {
		return 0, fmt.Errorf("expected(%v) actual(%v)", leadingCh, t1[0])
	}
	size, err := strconv.Atoi(string(t1[1:]))
	if err != nil {
		return 0, fmt.Errorf("parse size: %w", err)
	}
	return size, nil
}

func (h *Handler) readBulkString() (string, error) {
	size, err := h.readInt('$')
	if err != nil {
		return "", fmt.Errorf("read size: %w", err)
	}
	// TODO: the next string can contains `\r\n`, hence this way might not be correct.
	t2, err := h.nextToken()
	if err != nil {
		return "", fmt.Errorf("next token: %w", err)
	}
	if len(t2) != size {
		return "", fmt.Errorf("expected size: %d - %d", len(t2), size)
	}

	return string(t2), nil
}

func makeBulkString(input string) ([]byte, error) {
	data := []byte(input)

	var re []byte
	re = append(re, '$')
	re = append(re, []byte(strconv.Itoa(len(data)))...)
	re = append(re, '\r', '\n')
	re = append(re, data...)
	re = append(re, '\r', '\n')

	return re, nil
}
