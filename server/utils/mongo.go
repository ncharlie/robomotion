package utils

import (
	"context"
	"log/slog"
	"time"

	"go.mongodb.org/mongo-driver/mongo"
	"go.mongodb.org/mongo-driver/mongo/options"
)

func ptr[T any](value T) *T {
	return &value
}

func NewMongoConnection(conn string) *mongo.Client {
	ctx, cancelFunc := context.WithTimeout(context.Background(), 10*time.Second)
	defer cancelFunc()

	option := options.ClientOptions{
		MinPoolSize:     ptr[uint64](50),
		MaxPoolSize:     ptr[uint64](3000),
		MaxConnecting:   ptr[uint64](150),
		ConnectTimeout:  ptr(10 * time.Second),
		MaxConnIdleTime: ptr(1 * time.Second),
	}

	option.ApplyURI(conn)
	client, err := mongo.Connect(ctx, &option)
	if err != nil {
		panic(err)
	}

	if err := client.Ping(ctx, nil); err != nil {
		panic("Cannot connect to MongoDB: " + err.Error())
	}

	slog.Info("Connected to Mongo")
	return client
}

func DisconnectMongo(client *mongo.Client) error {
	if err := client.Disconnect(context.TODO()); err != nil {
		slog.Error("Mongo client disconnect error", "error", err)
		return err
	}
	slog.Warn("Mongo client disconnected")
	return nil
}
