# Robomotion

## Prerequisite

1. Clone the repo

```bash
git clone https://github.com/ncharlie/robomotion.git
cd robomotion/server
```

2. Setup configuration in config file

Put `config.yaml` in /server directory. The file contains:

```yaml
app:
  port: 3000
mongodb:
  connection: "mongodb://<username>:<password>@<host>:<port>/"
  db_name: "<database name>"
  log_collection: "logs"
  beacon_collection: "beacons"
  robot_collection: "robots"
mqtt:
  url: "mqtt://<host>:<port>"
  username: "<username>"
  password: "<password>"
  client_id: "<client id>"
  topic:
    location: "@msg/location/+"
    notification: "@msg/noti/+"
    movement: "@msg/move"
tls:
  cert: "<path to cert>"
  key: "<path to key>"
log:
  level: "debug"
  format: "text"
```

## Installation

You can build the docker image, create a container and run by running the `deploy.sh` script.

```bash
./deploy.sh
```

## Usage

```bash
docker container start robomotion
```
