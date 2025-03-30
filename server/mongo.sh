docker run --name robomongo --network=bridge \
    --restart=always \
    --log-opt max-size=1g --log-opt max-file=3 \
    -v dataMongo:/data/db \
    -e MONGO_INITDB_ROOT_USERNAME=robomotion \
    -e MONGO_INITDB_ROOT_PASSWORD=i0Tprojek@TggS \
    -p 3333:27017 \
    -d mongo:4.4.29
