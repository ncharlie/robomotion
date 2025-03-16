package utils

import (
	"context"
	"log/slog"
	"time"

	"go.mongodb.org/mongo-driver/mongo"
	"go.mongodb.org/mongo-driver/mongo/options"
)

const MongoClientTimeout = 5

func NewMongoConnection(conn string) *mongo.Client {
	ctx, cancelFunc := context.WithTimeout(context.Background(), MongoClientTimeout*time.Second)
	defer cancelFunc()

	client, err := mongo.Connect(ctx, options.Client().ApplyURI(conn))

	if err != nil {
		panic(err)
	}

	// defer func() {
	// 	if err := client.Disconnect(context.TODO()); err != nil {
	// 		panic(err)
	// 	}
	// }()

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
