# ----------------------------------------
IMAGE_NAME="robomotion"
CONTAINER_NAME="robomotion"
TAG="1.0.0"
DOCKER_NETWORK="bridge"

# ---------------------------------------------IMAGE
# REMOVE old docker image
sudo docker image rm --force $IMAGE_NAME:$TAG
# BUILD new image
DOCKER_BUILDKIT=1 sudo docker image build -t $IMAGE_NAME:$TAG .

# -----------------------------------------CONTAINER
# STOP running container
sudo docker container stop $CONTAINER_NAME
# REMOVE old docker container
sudo docker container rm --force $CONTAINER_NAME
# CREATE new container CONNECT to network
sudo docker create --name $CONTAINER_NAME \
    --network=$DOCKER_NETWORK \
    --restart=always \
    --log-opt max-size=1g --log-opt max-file=3 \
    -v ~/config.yaml:/app/config.yaml:ro \
    -v ~/tls:/app/tls:ro \
    -v $PWD/static:/app/static:ro \
    -p 443:8443 \
    $IMAGE_NAME:$TAG "$1"

# RUN the container
sudo docker start $CONTAINER_NAME
