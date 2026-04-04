package main

import (
	"net"

	"go.uber.org/zap"

	"github.com/codecrafters-io/redis-starter-go/app/server"
)

func main() {

	logger, err := zap.NewDevelopment()
	if err != nil {
		panic(err)
	}
	zap.ReplaceGlobals(logger)

	l, err := net.Listen("tcp", "0.0.0.0:6379")
	if err != nil {
		zap.L().Fatal("failed to listen to port", zap.Error(err))
	}
	defer func() {
		cErr := l.Close()
		if cErr != nil {
			zap.L().Error("failed to close listener", zap.Error(cErr))
		}
	}()

	zap.L().Info("start listening")
	s := server.NewServer()

	for {
		conn, err := l.Accept()
		if err != nil {
			zap.L().Error("failed to accept connection", zap.Error(err))
		}

		h := server.NewHandler(conn, s)
		go h.ListenAndServe()
	}
}
