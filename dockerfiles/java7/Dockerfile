FROM ubuntu:14.04

MAINTAINER ngocpq <phungquangngoc@gmail.com>

#############################################################################
# Requirements
#############################################################################

RUN apt-get update -y 
RUN apt-get install software-properties-common -y 
RUN apt-get update -y 
RUN apt-get install -y openjdk-7-jdk
RUN apt-get install -y \
                git \
                build-essential \
                subversion \
                perl \
                curl \
                unzip \
                cpanminus \
                make \
                locales \
                maven \
  && \
  rm -rf /var/lib/apt/lists/*

# Java version
ENV JAVA_HOME /usr/lib/jvm/java-7-openjdk-amd64

RUN locale-gen en_US.UTF-8
ENV LANG en_US.UTF-8
ENV LANGUAGE en_US:en
ENV LC_ALL en_US.UTF-8

# Timezone
ENV TZ=America/Los_Angeles
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone


#############################################################################
# Setup Defects4J
#############################################################################

# ----------- Step 1. Clone defects4j from github --------------
WORKDIR /
RUN git clone https://github.com/rjust/defects4j.git defects4j

# ----------- Step 2. Initialize Defects4J ---------------------
WORKDIR /defects4j
RUN cpanm --installdeps .
RUN ./init.sh

# ----------- Step 3. Add Defects4J's executables to PATH: ------
ENV PATH="/defects4j/framework/bin:${PATH}"  
#--------------
