package repositories

import (
	"context"
	"robomotion/env"
	"robomotion/models/entities"
	"time"

	"go.mongodb.org/mongo-driver/mongo"
)

type LogRepository struct {
	collection *mongo.Collection
}

func NewTransactionLogRepository(client *mongo.Client) *LogRepository {
	coll := client.Database(env.Get("mongodb.db_name")).Collection(env.Get("mongodb.transaction_log_collection"))

	return &LogRepository{
		collection: coll,
	}
}

func (r *LogRepository) InsertLog(log *entities.TransactionLog) error {
	var ctx, cancel = context.WithTimeout(context.Background(), 10*time.Second)
	defer cancel()

	log.CreatedAt = time.Now()

	_, err := r.collection.InsertOne(ctx, log, nil)
	// t, _ := time.Parse(time.RFC3339, "2023-05-10T16:29:40.333+07:00")

	return err
}
