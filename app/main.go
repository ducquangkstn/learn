package main

import (
	"fmt"
	"net"
	"os"

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
	defer l.Close()

	zap.L().Info("start listening")
	s := server.NewServer()

	for {
		conn, err := l.Accept()
		if err != nil {
			fmt.Println("Error accepting connection: ", err.Error())
			os.Exit(1)
		}

		h := server.NewHandler(conn, s)
		go h.ListenAndServe()
	}
}
