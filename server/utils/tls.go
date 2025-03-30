package utils

import (
	"crypto/tls"
	"log/slog"
	"os"
)

func BuildTLS(certPath, keyPath string) *tls.Config {
	crt, err := os.ReadFile(certPath)
	if err != nil {
		slog.Error("Error reading cert", "error", err)
		panic(err)
	}

	key, err := os.ReadFile(keyPath)
	if err != nil {
		slog.Error("Error reading key", "error", err)
		panic(err)
	}

	cert, err := tls.X509KeyPair(crt, key)
	if err != nil {
		slog.Error("Error pairing cert", "error", err)
		panic(err)
	}

	return &tls.Config{
		Certificates: []tls.Certificate{cert},
		ServerName:   "127.0.0.1",
	}
}
