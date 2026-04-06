package server

import (
	"fmt"
	"strconv"
	"strings"
	"time"

	"go.uber.org/zap"
)

func (h *Handler) ping(size int) error {
	if size != 1 {
		return fmt.Errorf("unexpected size: %d", size)
	}
	data := []byte("+PONG\r\n")
	_, err := h.conn.Write(data)
	if err != nil {
		return fmt.Errorf("conn.Write: %w", err)
	}
	return nil
}

func (h *Handler) echo(size int) error {
	if size != 2 {
		return fmt.Errorf("unexpected size: %d", size)
	}
	msg, err := h.readBulkString()
	if err != nil {
		return fmt.Errorf("readBulkString: %w", err)
	}
	data := makeBulkString(msg)
	_, err = h.conn.Write(data)
	if err != nil {
		return fmt.Errorf("conn.Write: %w", err)
	}
	return nil
}

func (h *Handler) set(size int) error {
	if size < 3 {
		return fmt.Errorf("unexpected size: %d", size)
	}
	t1, err := h.readBulkString()
	if err != nil {
		return fmt.Errorf("read 1st args: %w", err)
	}

	t2, err := h.readBulkString()
	if err != nil {
		return fmt.Errorf("read 2nd args: %w", err)
	}

	expired := int64(-1)

	for i := 3; i < size; i++ {
		fmt.Println("foobar", i, size)
		k, err := h.readBulkString()
		if err != nil {
			return fmt.Errorf("read 2nd args: %w", err)
		}
		switch k {
		case "EX", "PX":
			v, err := h.readBulkString()
			if err != nil {
				return err
			}

			vInt, err := strconv.ParseInt(v, 10, 64)
			if err != nil {
				return fmt.Errorf("parse expire: %w", err)
			}

			expired = vInt * int64(time.Second)
			if k == "PX" {
				expired = vInt * int64(time.Millisecond)
			}

			i++
		default:
			return fmt.Errorf("unexpected argus: %s", k)
		}
	}

	h.server.setWithExpired(t1, t2, time.Duration(expired))

	_, err = h.conn.Write([]byte("+OK\r\n"))
	if err != nil {
		return fmt.Errorf("conn.Write: %w", err)
	}
	return nil
}

func (h *Handler) get(size int) error {
	if size != 2 {
		return fmt.Errorf("unexpected size: %d", size)
	}
	t1, err := h.readBulkString()
	if err != nil {
		return fmt.Errorf("read 1st args: %w", err)
	}

	t2, ok := h.server.get(t1)
	if !ok {
		_, err = h.conn.Write([]byte("$-1\r\n"))
		if err != nil {
			return fmt.Errorf("conn.Write: %w", err)
		}
		return nil
	}

	data := makeBulkString(t2)
	_, err = h.conn.Write(data)
	if err != nil {
		return fmt.Errorf("conn.Write: %w", err)
	}
	return nil
}

func (h *Handler) getType(size int) error {
	if size != 2 {
		return fmt.Errorf("unexpected size: %d", size)
	}
	t1, err := h.readBulkString()
	if err != nil {
		return fmt.Errorf("read 1st args: %w", err)
	}
	zap.L().Info("type command", zap.String("key", t1))
	re := h.server.getType(t1)

	data := makeSimpleString(strings.ToLower(re.String()))
	zap.L().Info("resp type command:", zap.String("resp", string(data)))
	_, err = h.conn.Write(data)
	if err != nil {
		return fmt.Errorf("conn.Write: %w", err)
	}
	return nil
}
