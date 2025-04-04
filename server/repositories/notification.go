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

type NotificationRepository struct {
	collection *mongo.Collection
}

func NewNotificationRepository(client *mongo.Client) *NotificationRepository {
	coll := client.Database(env.Get("mongodb.db_name")).Collection(env.Get("mongodb.notification_collection"))

	repo := NotificationRepository{
		collection: coll,
	}

	repo.CreateIndex()
	return &repo
}

func (r *NotificationRepository) Insert(noti *entities.Notification) error {
	var ctx, cancel = context.WithTimeout(context.Background(), 10*time.Second)
	defer cancel()

	noti.CreatedAt = time.Now()

	_, err := r.collection.InsertOne(ctx, noti, nil)

	return err
}

func (r *NotificationRepository) CreateIndex() error {
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

	slog.Info("Index created for Noti Repo", "name", result)
	return err
}
