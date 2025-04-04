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

type RobotRepository struct {
	collection *mongo.Collection
}

func NewRobotRepository(client *mongo.Client) *RobotRepository {
	coll := client.Database(env.Get("mongodb.db_name")).Collection(env.Get("mongodb.robot_collection"))

	repo := RobotRepository{
		collection: coll,
	}

	repo.CreateIndex()
	return &repo
}

func (r *RobotRepository) Insert(robot *entities.Robot) error {
	var ctx, cancel = context.WithTimeout(context.Background(), 10*time.Second)
	defer cancel()

	robot.CreatedAt = time.Now()

	_, err := r.collection.InsertOne(ctx, robot, nil)

	return err
}

func (r *RobotRepository) GetRobot(id string) *entities.Robot {
	var ctx, cancel = context.WithTimeout(context.Background(), 10*time.Second)
	defer cancel()

	opt := options.FindOne()

	result := r.collection.FindOne(ctx, bson.M{"robot_id": id}, opt)
	if result.Err() != nil {
		slog.Error("Error getting robot", "error", result.Err())
		return nil
	}

	var robot entities.Robot
	err := result.Decode(&robot)
	if err != nil {
		slog.Error("Error decoding robot", "error", err)
		return nil
	}

	return &robot
}

func (r *RobotRepository) CreateIndex() error {
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
			Keys: bson.D{{"updated_at", -1}},
			Options: &options.IndexOptions{
				Name: ptr("time"),
			},
		},
	}
	result, err := r.collection.Indexes().CreateMany(ctx, indexes)

	if err != nil {
		panic(err)
	}

	slog.Info("Index created for Robot Repo", "name", result)
	return err
}
