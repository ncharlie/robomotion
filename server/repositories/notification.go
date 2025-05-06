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

func (r *NotificationRepository) GetNotification(id string, last *time.Time) *entities.Notification {
	var ctx, cancel = context.WithTimeout(context.Background(), 10*time.Second)
	defer cancel()

	opt := options.FindOne().SetSort(bson.M{"created_at": 1})
	filter := bson.M{
		"robot_id": id,
		"created_at": bson.M{
			"$gt": last,
		},
	}

	var noti entities.Notification
	if err := r.collection.FindOne(ctx, filter, opt).Decode(&noti); err != nil {
		if err != mongo.ErrNoDocuments {
			slog.Error("Error getting notification", "error", err)
		}
		return nil
	}

	return &noti
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
