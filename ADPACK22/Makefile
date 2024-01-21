build: Dockerfile
	docker build -t adpack22:1.0 --progress=plain .

build-server: Dockerfile
	sudo docker build -t adpack22:1.0 --progress=plain .

run:
	docker container run --rm -it adpack22:1.0

run-server:
	sudo docker container run --rm -it adpack22:1.0