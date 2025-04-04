package repositories

import (
	"context"
	"log/slog"
	"robomotion/env"
	"robomotion/models/entities"
	"time"

	"go.mongodb.org/mongo-driver/bson"
	"go.mongodb.org/mongo-driver/mongo"
	"go.mongodb.org/mongo-driver/mongo/options"
)

func ptr[T any](value T) *T {
	return &value
}

type LogRepository struct {
	collection *mongo.Collection
}

func NewLogRepository(client *mongo.Client) *LogRepository {
	coll := client.Database(env.Get("mongodb.db_name")).Collection(env.Get("mongodb.log_collection"))

	repo := LogRepository{
		collection: coll,
	}

	repo.CreateIndex()
	return &repo
}

func (r *LogRepository) InsertLog(update *entities.Update) error {
	var ctx, cancel = context.WithTimeout(context.Background(), 10*time.Second)
	defer cancel()

	update.CreatedAt = time.Now()

	_, err := r.collection.InsertOne(ctx, update)
	// t, _ := time.Parse(time.RFC3339, "2023-05-10T16:29:40.333+07:00")

	return err
}

func (r *LogRepository) GetLatestLocation(id string) *entities.Update {
	var ctx, cancel = context.WithTimeout(context.Background(), 10*time.Second)
	defer cancel()

	opt := options.FindOne().SetSort(bson.M{"created_at": -1})
	filter := bson.M{
		"robot_id": id,
		"created_at": bson.M{
			"$gte": time.Now().Add(-time.Second * 10),
		},
	}

	var update entities.Update
	if err := r.collection.FindOne(ctx, filter, opt).Decode(&update); err != nil {
		if err != mongo.ErrNoDocuments {
			slog.Error("Error getting location", "error", err)
		}
		return nil
	}

	return &update
}

func (r *LogRepository) CreateIndex() error {
	var ctx, cancel = context.WithTimeout(context.Background(), 10*time.Second)
	defer cancel()

	indexes := []mongo.IndexModel{
		{
			Keys: bson.D{{"robot_id", 1}},
			Options: &options.IndexOptions{
				Name: ptr("robot_id"),
			},
		},
		{
			Keys: bson.D{{"created_at", -1}},
			Options: &options.IndexOptions{
				Name:               ptr("time"),
				ExpireAfterSeconds: ptr(int32(259200)), // 3 days
			},
		},
	}
	result, err := r.collection.Indexes().CreateMany(ctx, indexes)

	if err != nil {
		panic(err)
	}

	slog.Info("Index created for Log Repo", "name", result)
	return err
}
