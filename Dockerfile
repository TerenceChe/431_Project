FROM ubuntu:latest

# Install necessary packages
RUN apt-get update && \
    apt-get install -y \
    build-essential \
    mpich \
    make \
    && rm -rf /var/lib/apt/lists/*

# Set the working directory inside the container
WORKDIR /cmpt431_project

# Copy the current directory contents into the container at /app
COPY . .

# Run any commands needed to build your project
# For example, if you have a Makefile, you can simply run `make`
CMD ["make all"]