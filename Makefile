.PHONY: clean docker example

all:
	cmake -Bbuild -DCMAKE_INSTALL_PREFIX=install
	cmake --build build --config Release
	cmake --install build

docker:
	docker buildx build . --platform linux/amd64,linux/arm64,linux/arm/v7 --output 'type=local,dest=dist'

example:
	cmake -Bbuild/example -Sexample -DCMAKE_INSTALL_PREFIX=install
	cmake --build build/example --config Release
	cmake --install build/example

clean:
	rm -rf build install dist