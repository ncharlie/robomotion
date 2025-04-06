package main

import (
	"context"
	"log"
	"log/slog"
	"net/http"
	"os"
	"os/signal"
	"robomotion/env"
	"robomotion/handlers"
	"robomotion/repositories"
	"robomotion/services"
	"robomotion/utils"
	"syscall"
	"time"

	"github.com/gorilla/mux"
)

func main() {
	env.SetupConfig()

	defer slog.Info("Done cleaning up")
	slog.Info("starting...")

	dbConn := utils.NewMongoConnection(env.Get("mongodb.connection"))
	defer utils.DisconnectMongo(dbConn)
	mqttConn := utils.NewMqttClient(
		env.Get("mqtt.url"),
		env.Get("mqtt.username"),
		env.Get("mqtt.password"),
		env.Get("mqtt.client_id"),
	)
	defer utils.DisconnectMqtt(mqttConn)

	robotRepo := repositories.NewRobotRepository(dbConn)
	beaconRepo := repositories.NewBeaconRepository(dbConn)
	logRepo := repositories.NewLogRepository(dbConn)
	notiRepo := repositories.NewNotificationRepository(dbConn)
	service := services.NewService(robotRepo, beaconRepo, logRepo, notiRepo)
	handler := handlers.NewSubscribeHandler(service)
	_ = services.NewMqttService(logRepo, mqttConn)

	r := mux.NewRouter()
	// r.HandleFunc("/search/{searchTerm}", Search)
	r.HandleFunc("/auth", handler.Auth).Methods("POST")
	r.HandleFunc("/update", handler.GetParams).Methods("GET")
	static := http.StripPrefix("/static/", http.FileServer(http.Dir("./static/")))
	r.PathPrefix("/static/").Handler(static)

	server := &http.Server{
		Addr:    "0.0.0.0:" + env.Get("app.port"),
		Handler: r,

		ReadTimeout:  15 * time.Second,
		WriteTimeout: time.Second * 15,
		IdleTimeout:  time.Second * 60,
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

	slog.Info("start listening http1 on 0.0.0.0:" + env.Get("app.port"))
	if err := server.ListenAndServeTLS(env.Get("tls.cert"), env.Get("tls.key")); err != nil && err != http.ErrServerClosed {
		slog.Error("Error starting server", "error", err)
		log.Fatal(err)
	}

	slog.Info("Robomotion server has stopped")
	slog.Info("Running cleanup tasks...")
}
