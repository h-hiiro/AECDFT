build: Dockerfile
	docker build -t hhiiro/aecdft:1.0 .

debug: Dockerfile
	docker build -t hhiiro/aecdft:1.0 --progress=plain .


run:
	docker run -it --rm -v "D:/Win10/Documents/AECDFT/AECDFT/Input":/work/Input -v "D:/Win10/Documents/AECDFT/AECDFT/Output":/work/Output hhiiro/aecdft:1.0 