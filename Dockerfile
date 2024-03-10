FROM gcc:latest

RUN apt-get update && apt-get install -y libpqxx-dev libboost-dev cmake

WORKDIR /app

COPY crow-v1.0+5.deb .

RUN dpkg -i crow-v1.0+5.deb

COPY . .

# RUN g++ -std=c++20 -o main main.cpp -lpqxx -lpq
RUN cmake .

RUN make

CMD ["./backend"]
