#include "ngx_http_waf_module_macro.h"
#include "ngx_http_waf_module_type.h"
#include <stdio.h>

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#ifndef NGX_HTTP_WAF_MODULE_CONFIG_H
#define NGX_HTTP_WAF_MODULE_CONFIG_H

static char* ngx_http_waf_mult_mount_conf(ngx_conf_t* cf, ngx_command_t* cmd, void* conf);


static char* ngx_http_waf_conf(ngx_conf_t* cf, ngx_command_t* cmd, void* conf);


static char* ngx_http_waf_rule_path_conf(ngx_conf_t* cf, ngx_command_t* cmd, void* conf);


static char* ngx_http_waf_mode_conf(ngx_conf_t* cf, ngx_command_t* cmd, void* conf);


static char* ngx_http_waf_cc_deny_limit_conf(ngx_conf_t* cf, ngx_command_t* cmd, void* conf);


static ngx_int_t ngx_http_waf_init_after_load_config(ngx_conf_t* cf);


static char* ngx_http_waf_mult_mount_conf(ngx_conf_t* cf, ngx_command_t* cmd, void* conf) {
    if (ngx_conf_set_flag_slot(cf, cmd, conf) != NGX_CONF_OK) {
        return NGX_CONF_ERROR;
    }
    return NGX_CONF_OK;
}


static char* ngx_http_waf_conf(ngx_conf_t* cf, ngx_command_t* cmd, void* conf) {
    if (ngx_conf_set_flag_slot(cf, cmd, conf) != NGX_CONF_OK) {
        return NGX_CONF_ERROR;
    }
    return NGX_CONF_OK;
}


static char* ngx_http_waf_rule_path_conf(ngx_conf_t* cf, ngx_command_t* cmd, void* conf) {
    ngx_http_waf_srv_conf_t* srv_conf = conf;
    if (ngx_conf_set_str_slot(cf, cmd, conf) != NGX_CONF_OK) {
        ngx_conf_log_error(NGX_LOG_ERR, cf, 0, "ngx_waf: %s", "The path of the config file is not specified");
        return NGX_CONF_ERROR;
    }

    char* full_path = ngx_palloc(cf->pool, sizeof(char) * RULE_MAX_LEN);
    char* end = to_c_str((u_char*)full_path, srv_conf->waf_rule_path);

    CHECK_AND_LOAD_CONF(cf, full_path, end, IPV4_FILE, srv_conf->black_ipv4, 1);
    CHECK_AND_LOAD_CONF(cf, full_path, end, URL_FILE, srv_conf->black_url, 0);
    CHECK_AND_LOAD_CONF(cf, full_path, end, ARGS_FILE, srv_conf->black_args, 0);
    CHECK_AND_LOAD_CONF(cf, full_path, end, UA_FILE, srv_conf->black_ua, 0);
    CHECK_AND_LOAD_CONF(cf, full_path, end, REFERER_FILE, srv_conf->black_referer, 0);
    CHECK_AND_LOAD_CONF(cf, full_path, end, COOKIE_FILE, srv_conf->black_cookie, 0);
    CHECK_AND_LOAD_CONF(cf, full_path, end, POST_FILE, srv_conf->black_post, 0);
    CHECK_AND_LOAD_CONF(cf, full_path, end, WHITE_IPV4_FILE, srv_conf->white_ipv4, 1);
    CHECK_AND_LOAD_CONF(cf, full_path, end, WHITE_URL_FILE, srv_conf->white_url, 0);
    CHECK_AND_LOAD_CONF(cf, full_path, end, WHITE_REFERER_FILE, srv_conf->white_referer, 0);

    ngx_pfree(cf->pool, full_path);
    return NGX_CONF_OK;
}


static char* ngx_http_waf_mode_conf(ngx_conf_t* cf, ngx_command_t* cmd, void* conf) {
    ngx_http_waf_srv_conf_t* srv_conf = (ngx_http_waf_srv_conf_t*)conf;
    ngx_str_t* modes = cf->args->elts;

    for (size_t i = 1; i < cf->args->nelts && modes != NULL; i++) {
        if (ngx_strncasecmp(modes[i].data, (u_char*)"GET", min(modes[i].len, 5)) == 0) {
            srv_conf->waf_mode |= MODE_INSPECT_GET;
        } 
        else if (ngx_strncasecmp(modes[i].data, (u_char*)"HEAD", min(modes[i].len, 5)) == 0) {
            srv_conf->waf_mode |= MODE_INSPECT_HEAD;
        } 
        else if (ngx_strncasecmp(modes[i].data, (u_char*)"POST", min(modes[i].len, 5)) == 0) {
            srv_conf->waf_mode |= MODE_INSPECT_POST;
        }
        else if (ngx_strncasecmp(modes[i].data, (u_char*)"PUT", min(modes[i].len, 4)) == 0) {
            srv_conf->waf_mode |= MODE_INSPECT_PUT;
        }
        else if (ngx_strncasecmp(modes[i].data, (u_char*)"DELETE", min(modes[i].len, 7)) == 0) {
            srv_conf->waf_mode |= MODE_INSPECT_DELETE;
        }
        else if (ngx_strncasecmp(modes[i].data, (u_char*)"MKCOL", min(modes[i].len, 6)) == 0) {
            srv_conf->waf_mode |= MODE_INSPECT_MKCOL;
        }
        else if (ngx_strncasecmp(modes[i].data, (u_char*)"COPY", min(modes[i].len, 5)) == 0) {
            srv_conf->waf_mode |= MODE_INSPECT_COPY;
        }
        else if (ngx_strncasecmp(modes[i].data, (u_char*)"MOVE", min(modes[i].len, 5)) == 0) {
            srv_conf->waf_mode |= MODE_INSPECT_MOVE;
        }
        else if (ngx_strncasecmp(modes[i].data, (u_char*)"OPTIONS", min(modes[i].len, 8)) == 0) {
            srv_conf->waf_mode |= MODE_INSPECT_OPTIONS;
        }
        else if (ngx_strncasecmp(modes[i].data, (u_char*)"PROPFIND", min(modes[i].len, 9)) == 0) {
            srv_conf->waf_mode |= MODE_INSPECT_PROPFIND;
        }
        else if (ngx_strncasecmp(modes[i].data, (u_char*)"PROPPATCH", min(modes[i].len, 10)) == 0) {
            srv_conf->waf_mode |= MODE_INSPECT_PROPPATCH;
        }
        else if (ngx_strncasecmp(modes[i].data, (u_char*)"LOCK", min(modes[i].len, 5)) == 0) {
            srv_conf->waf_mode |= MODE_INSPECT_LOCK;
        }
        else if (ngx_strncasecmp(modes[i].data, (u_char*)"UNLOCK", min(modes[i].len, 7)) == 0) {
            srv_conf->waf_mode |= MODE_INSPECT_UNLOCK;
        }
        else if (ngx_strncasecmp(modes[i].data, (u_char*)"PATCH", min(modes[i].len, 6)) == 0) {
            srv_conf->waf_mode |= MODE_INSPECT_PATCH;
        }
        else if (ngx_strncasecmp(modes[i].data, (u_char*)"TRACE", min(modes[i].len, 6)) == 0) {
            srv_conf->waf_mode |= MODE_INSPECT_TRACE;
        }
        else if (ngx_strncasecmp(modes[i].data, (u_char*)"IP", min(modes[i].len, 3)) == 0) {
            srv_conf->waf_mode |= MODE_INSPECT_IP;
        }
        else if (ngx_strncasecmp(modes[i].data, (u_char*)"URL", min(modes[i].len, 4)) == 0) {
            srv_conf->waf_mode |= MODE_INSPECT_URL;
        }
        else if (ngx_strncasecmp(modes[i].data, (u_char*)"RBODY", min(modes[i].len, 6)) == 0) {
            srv_conf->waf_mode |= MODE_INSPECT_RB;
        }
        else if (ngx_strncasecmp(modes[i].data, (u_char*)"ARGS", min(modes[i].len, 5)) == 0) {
            srv_conf->waf_mode |= MODE_INSPECT_ARGS;
        }
        else if (ngx_strncasecmp(modes[i].data, (u_char*)"UA", min(modes[i].len, 3)) == 0) {
            srv_conf->waf_mode |= MODE_INSPECT_UA;
        }
        else if (ngx_strncasecmp(modes[i].data, (u_char*)"COOKIE", min(modes[i].len, 7)) == 0) {
            srv_conf->waf_mode |= MODE_INSPECT_COOKIE;
        }
        else if (ngx_strncasecmp(modes[i].data, (u_char*)"REFERER", min(modes[i].len, 8)) == 0) {
            srv_conf->waf_mode |= MODE_INSPECT_REFERER;
        }
        else if (ngx_strncasecmp(modes[i].data, (u_char*)"CC", min(modes[i].len, 3)) == 0) {
            srv_conf->waf_mode |= MODE_INSPECT_CC;
        }
        else if (ngx_strncasecmp(modes[i].data, (u_char*)"STD", min(modes[i].len, 4)) == 0) {
            srv_conf->waf_mode |= MODE_STD;
        }
        else if (ngx_strncasecmp(modes[i].data, (u_char*)"FULL", min(modes[i].len, 5)) == 0) {
            srv_conf->waf_mode |= MODE_FULL;
        }
        else {
            ngx_log_error(NGX_LOG_ERR, cf->log, 0, "Invalid value");
            return NGX_CONF_ERROR;
        }
    }

    return NGX_CONF_OK;
}


static char* ngx_http_waf_cc_deny_limit_conf(ngx_conf_t* cf, ngx_command_t* cmd, void* conf) {
    ngx_http_waf_srv_conf_t* srv_conf = conf;
    ngx_str_t* p_str = cf->args->elts;
    srv_conf->waf_cc_deny_limit = ngx_atoi((p_str + 1)->data, (p_str + 1)->len);
    srv_conf->waf_cc_deny_duration = ngx_atoi((p_str + 2)->data, (p_str + 2)->len);
    if (srv_conf->waf_cc_deny_limit <= 0 || srv_conf->waf_cc_deny_duration <= 0) {
        ngx_log_error(NGX_LOG_ERR, cf->log, 0, "Cannot be converted to an integer greater than zero %d %d",
            srv_conf->waf_cc_deny_limit,
            srv_conf->waf_cc_deny_duration);
        return NGX_CONF_ERROR;
    }
    return NGX_CONF_OK;
}


static void* ngx_http_waf_create_srv_conf(ngx_conf_t* cf) {
    ngx_http_waf_srv_conf_t* srv_conf = NULL;
    srv_conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_waf_srv_conf_t));
    if (srv_conf == NULL) {
        return NULL;
    }
    ngx_str_null(&srv_conf->waf_rule_path);
    srv_conf->ngx_log = ngx_log_init(NULL);
    srv_conf->ngx_pool = ngx_create_pool(sizeof(ngx_pool_t) + INITIAL_SIZE, srv_conf->ngx_log);
    srv_conf->alloc_times = 0;
    srv_conf->waf = NGX_CONF_UNSET;
    srv_conf->waf_mult_mount = NGX_CONF_UNSET;
    srv_conf->waf_mode = 0;
    srv_conf->waf_cc_deny_limit = NGX_CONF_UNSET;
    srv_conf->waf_cc_deny_duration = NGX_CONF_UNSET;
    srv_conf->black_ipv4 = ngx_array_create(cf->pool, 10, sizeof(ipv4_t));
    srv_conf->black_url = ngx_array_create(cf->pool, 10, sizeof(ngx_regex_elt_t));
    srv_conf->black_args = ngx_array_create(cf->pool, 10, sizeof(ngx_regex_elt_t));
    srv_conf->black_ua = ngx_array_create(cf->pool, 10, sizeof(ngx_regex_elt_t));
    srv_conf->black_referer = ngx_array_create(cf->pool, 10, sizeof(ngx_regex_elt_t));
    srv_conf->black_cookie = ngx_array_create(cf->pool, 10, sizeof(ngx_regex_elt_t));
    srv_conf->black_post = ngx_array_create(cf->pool, 10, sizeof(ngx_regex_elt_t));
    srv_conf->white_ipv4 = ngx_array_create(cf->pool, 10, sizeof(ipv4_t));
    srv_conf->white_url = ngx_array_create(cf->pool, 10, sizeof(ngx_regex_elt_t));
    srv_conf->white_referer = ngx_array_create(cf->pool, 10, sizeof(ngx_regex_elt_t));
    srv_conf->ipv4_times = NULL;

    if (srv_conf->ngx_log == NULL
        || srv_conf->ngx_pool == NULL
        || srv_conf->black_ipv4 == NULL
        || srv_conf->black_url == NULL
        || srv_conf->black_args == NULL
        || srv_conf->black_ua == NULL
        || srv_conf->black_referer == NULL
        || srv_conf->white_ipv4 == NULL
        || srv_conf->white_url == NULL
        || srv_conf->white_referer == NULL) {
        ngx_log_error(NGX_LOG_ERR, cf->log, 0, "ngx_waf: Initialization failed");
        return NULL;
    }

    return srv_conf;
}


static ngx_int_t ngx_http_waf_init_after_load_config(ngx_conf_t* cf) {
    ngx_http_handler_pt* h;
    ngx_http_core_main_conf_t* cmcf;

    cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);
    h = ngx_array_push(&cmcf->phases[NGX_HTTP_PREACCESS_PHASE].handlers);
    if (h == NULL) {
        return NGX_ERROR;
    }
    *h = ngx_http_waf_handler_ip_url_referer_ua_args_cookie_post;

    h = ngx_array_push(&cmcf->phases[NGX_HTTP_SERVER_REWRITE_PHASE].handlers);
    *h = ngx_http_waf_handler_url_args;

    ngx_str_t waf_blocked_name = ngx_string("waf_blocked");
    ngx_http_variable_t* waf_blocked = ngx_http_add_variable(cf, &waf_blocked_name, NGX_HTTP_VAR_NOCACHEABLE);
    waf_blocked->get_handler = ngx_http_waf_blocked_get_handler;
    waf_blocked->set_handler = NULL;

    ngx_str_t waf_rule_type_name = ngx_string("waf_rule_type");
    ngx_http_variable_t* waf_rule_type = ngx_http_add_variable(cf, &waf_rule_type_name, NGX_HTTP_VAR_NOCACHEABLE);
    waf_rule_type->get_handler = ngx_http_waf_rule_type_get_handler;
    waf_rule_type->set_handler = NULL;

    ngx_str_t waf_rule_details_name = ngx_string("waf_rule_deatails");
    ngx_http_variable_t* waf_rule_details = ngx_http_add_variable(cf, &waf_rule_details_name, NGX_HTTP_VAR_NOCACHEABLE);
    waf_rule_details->get_handler = ngx_http_waf_rule_deatils_handler;
    waf_rule_details->set_handler = NULL;

    return NGX_OK;
}

#endif // !NGX_HTTP_WAF_MODULE_CONFIG_H
