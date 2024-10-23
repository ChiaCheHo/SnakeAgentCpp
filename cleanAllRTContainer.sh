docker stop $(docker ps -q)
docker rm $(docker ps -a -q)
docker rm $(docker ps -a -q --filter ancestor=rllab)