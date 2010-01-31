/* velox: modules/mpd.c
 *
 * Copyright (c) 2010 Michael Forney <michael@obberon.com>
 *
 * This file is a part of velox.
 *
 * velox is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License version 2, as published by the Free
 * Software Foundation.
 *
 * velox is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along
 * with velox.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <mpd/client.h>
#include <yaml.h>

#include <velox/velox.h>
#include <velox/module.h>

const char name[] = "mpd";

static const char * host = "localhost";
static uint16_t port = 6600;
static uint16_t timeout = 2000;

static struct mpd_connection * mpd_c;

static void play_pause();
static void next();
static void previous();
static void stop();

static bool check_error()
{
    if (mpd_connection_get_error(mpd_c) != MPD_ERROR_SUCCESS)
    {
        fprintf(stderr, "\nMPD: %s\n", mpd_connection_get_error_message(mpd_c));

        return false;
    }
    else
    {
        return true;
    }
}

void configure(yaml_document_t * document)
{
    yaml_node_t * map;
    yaml_node_pair_t * pair;

    yaml_node_t * key, * value;

    printf("MPD: Loading configuration...");

    map = document->nodes.start;
    assert(map->type == YAML_MAPPING_NODE);

    for (pair = map->data.mapping.pairs.start;
        pair < map->data.mapping.pairs.top;
        ++pair)
    {
        key = yaml_document_get_node(document, pair->key);
        value = yaml_document_get_node(document, pair->value);

        assert(key->type == YAML_SCALAR_NODE);

        if (strcmp((const char const *) key->data.scalar.value, "host") == 0)
        {
            assert(value->type == YAML_SCALAR_NODE);
            host = strdup((const char const *) value->data.scalar.value);
        }
        else if (strcmp((const char const *) key->data.scalar.value, "port") == 0)
        {
            assert(value->type == YAML_SCALAR_NODE);
            port = atoi((const char const *) value->data.scalar.value);
        }
        else if (strcmp((const char const *) key->data.scalar.value, "timeout") == 0)
        {
            assert(value->type == YAML_SCALAR_NODE);
            timeout = atoi((const char const *) value->data.scalar.value);
        }
    }

    printf("done\n\tHost: %s\n\tPort: %u\n\tTimeout: %u\n", host, port, timeout);
}

bool initialize()
{
    printf("MPD: Initializing module...");

    mpd_c = mpd_connection_new(host, port, timeout);
    assert(mpd_c);

    if (!check_error) return false;

    MODULE_KEYBINDING(play_pause)
    MODULE_KEYBINDING(next)
    MODULE_KEYBINDING(previous)
    MODULE_KEYBINDING(stop)

    printf("done\n");

    return true;
}

void cleanup()
{
    printf("MPD: Cleaning up module...");

    mpd_connection_free(mpd_c);

    printf("done\n");
}

static void play_pause()
{
    mpd_run_toggle_pause(mpd_c);
    check_error();
}

static void next()
{
    mpd_run_next(mpd_c);
    check_error();
}

static void previous()
{
    mpd_run_previous(mpd_c);
    check_error();
}

static void stop()
{
    mpd_run_stop(mpd_c);
    check_error();
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

