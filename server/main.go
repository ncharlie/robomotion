package main

import (
	"context"
	"log"
	"log/slog"
	"net/http"
	"os"
	"os/signal"
	"robomotion/env"
	"syscall"
	"time"

	"github.com/gorilla/mux"
)

func main() {
	env.SetupConfig()

	defer slog.Info("Done cleaning up")
	slog.Info("starting...")

	// dbConn := utils.NewMongoConnection(env.Get("mongodb.connection"))

	r := mux.NewRouter()
	// r.HandleFunc("/search/{searchTerm}", Search)
	static := http.StripPrefix("/static/", http.FileServer(http.Dir("./static/")))
	r.PathPrefix("/static/").Handler(static)

	server := &http.Server{
		Addr:        ":" + env.Get("app.port"),
		ReadTimeout: 15 * time.Second,
		Handler:     r,
	}

	exit := make(chan os.Signal, 1)
	signal.Notify(exit, syscall.SIGTERM, syscall.SIGINT)
	go func() {
		sig := <-exit
		slog.Info("Caught interrupt signal", "signal", sig)
		slog.Info("Shutting down listener")

		ctx, cancel := context.WithTimeout(context.Background(), 10*time.Second)
		defer cancel()

		if err := server.Shutdown(ctx); err != nil {
			slog.Error("Error shutting down server", "error", err)
		}
	}()

	slog.Info("Robomotion server is running")

	if err := server.ListenAndServe(); err != nil && err != http.ErrServerClosed {
		slog.Error("Error starting server", "error", err)
		log.Fatal(err)
	}

	slog.Info("Robomotion server has stopped")
	slog.Info("Running cleanup tasks...")
	// utils.DisconnectMongo(dbConn)
}
