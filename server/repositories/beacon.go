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

type BeaconRepository struct {
	collection *mongo.Collection
}

func NewBeaconRepository(client *mongo.Client) *BeaconRepository {
	coll := client.Database(env.Get("mongodb.db_name")).Collection(env.Get("mongodb.beacon_collection"))

	repo := BeaconRepository{
		collection: coll,
	}

	repo.CreateIndex()
	return &repo
}

func (r *BeaconRepository) Insert(robot *entities.Beacon) error {
	var ctx, cancel = context.WithTimeout(context.Background(), 10*time.Second)
	defer cancel()

	robot.CreatedAt = time.Now()

	_, err := r.collection.InsertOne(ctx, robot, nil)

	return err
}

func (r *BeaconRepository) GetBeacons() []entities.Beacon {
	var ctx, cancel = context.WithTimeout(context.Background(), 10*time.Second)
	defer cancel()

	opt := options.Find().SetSort(bson.M{"beacon_id": 1})

	cur, err := r.collection.Find(ctx, bson.M{}, opt)
	if err != nil {
		slog.Error("Error getting beacons", "error", err)
		return nil
	}
	defer cur.Close(ctx)
	var beacons []entities.Beacon

	if err = cur.All(ctx, &beacons); err != nil {
		slog.Error("Error decoding beacons", "error", err)
		return nil
	}
	return beacons
}

func (r *BeaconRepository) CreateIndex() error {
	var ctx, cancel = context.WithTimeout(context.Background(), 10*time.Second)
	defer cancel()

	indexes := []mongo.IndexModel{
		{
			Keys: bson.D{{"beacon_id", 1}},
			Options: &options.IndexOptions{
				Name: ptr("beacon_id"),
			},
		},
	}
	result, err := r.collection.Indexes().CreateMany(ctx, indexes)

	if err != nil {
		panic(err)
	}

	slog.Info("Index created for Beacon Repo", "name", result)
	return err
}
