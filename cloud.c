/* Copyright (C) 2022 Mateus de Lima Oliveira */

#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <json-c/json.h>
#include "libpq-fe.h"

static const char dbname[] = "dbname = ativarsoft";

static int route_login()
{
	PGconn *conn = NULL;
	struct json_object *parsed_json = NULL;
	char *lenenv = NULL;
	char *postdata = NULL;
	int ret = -1;
	int len = 0;

	lenenv = getenv("CONTENT_LENGTH");
	if (lenenv == NULL) {
		fputs("Could not find CONTENT_LENGTH environment variable.\n",
			stderr);
		return 1;
	}
	len = strtol(lenenv, NULL, 10);
	postdata = malloc(len + 1);
	if (postdata == NULL) {
		fprintf(stderr,
			"Not enough memory (%d bytes requested).\n",
			len + 1);
		return 1;
	}
	memset(postdata, 0, len + 1);
	fread(postdata, 1, len, stdin);

	parsed_json = json_tokener_parse(postdata);
	if (parsed_json == NULL) {
		fputs("Invalid JSON input.\n", stderr);
		ret = 1;
		goto out;
	}

	conn = PQconnectdb(dbname);
	/* Check to see that the backend connection was successfully made */
	if (PQstatus(conn) != CONNECTION_OK) {
		fputs(PQerrorMessage(conn), stderr);
		putc('\n', stderr);
		ret = 1;
		goto out;
	}
	ret = 0;
out:
	free(postdata);
	return ret;
}

int main(int argc, char **argv)
{
	char *pathinfo = NULL;
	char *s = NULL;

	pathinfo = getenv("PATH_INFO");
	if (pathinfo == NULL) {
		fputs("Could not find PATH_INFO environment variable.\n",
			stderr);
		return 1;
	}
	if (pathinfo[0] != '/') {
		fprintf(stderr, "Invalid path: %s\n", pathinfo);
		return 1;
	}
	s = strtok(pathinfo, "/");
	if (s == NULL) {
		fprintf(stderr, "Invalid path: %s\n", pathinfo);
		return 1;
	}
	if (strcmp(s, "login") == 0) {
		route_login();
	} else {
		fprintf(stderr, "Route not found: %s\n", s);
	}
	return 0;
}
