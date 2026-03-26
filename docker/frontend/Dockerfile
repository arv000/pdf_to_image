FROM node:18 AS build

WORKDIR /app

RUN npm install && npm run build
COPY . .
FROM nginx:alpine
COPY --from=build /app/dist /usr/share/nginx/html