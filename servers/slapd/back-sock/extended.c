/* modrdn.c - sock backend modrdn function */
/* $OpenLDAP$ */
/* This work is part of OpenLDAP Software <http://www.openldap.org/>.
 *
 * Copyright 2007-2010 The OpenLDAP Foundation.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted only as authorized by the OpenLDAP
 * Public License.
 *
 * A copy of this license is available in the file LICENSE in the
 * top-level directory of the distribution or, alternatively, at
 * <http://www.OpenLDAP.org/license.html>.
 */
/* ACKNOWLEDGEMENTS:
 * This work was initially developed by Brian Candler for inclusion
 * in OpenLDAP Software.
 */

#include "portable.h"

#include <stdio.h>

#include <ac/socket.h>
#include <ac/string.h>

#include "slap.h"
#include "back-sock.h"

int
sock_back_extended(
    Operation	*op,
    SlapReply	*rs )
{
	struct sockinfo	*si = (struct sockinfo *) op->o_bd->be_private;
	AttributeDescription *entry = slap_schema.si_ad_entry;
	Entry e;
	FILE			*fp;
	json_t          *result;
	json_t          *params;
	json_t          *json_request;
	int             err;
	json_error_t    error;
	struct berval	passwd_oid = BER_BVC(LDAP_EXOP_MODIFY_PASSWD);

	e.e_id = NOID;
	e.e_name = op->o_req_dn;
	e.e_nname = op->o_req_ndn;
	e.e_attrs = NULL;
	e.e_ocflags = 0;
	e.e_bv.bv_len = 0;
	e.e_bv.bv_val = NULL;
	e.e_private = NULL;

	if ( (fp = opensock( si->si_sockpath )) == NULL ) {
		send_ldap_error( op, rs, LDAP_OTHER,
		    "could not open socket" );
		return( -1 );
	}


	// if ( bvmatch( BER_BVC(LDAP_EXOP_MODIFY_PASSWD), &op->oq_extended.rs_reqoid ) ) {
	if ( bvmatch( &passwd_oid, &op->oq_extended.rs_reqoid ) ) {
		/* password modify RFC 3062 */

		/* write out the request to the modrdn process */
		req_pwdexop_s *qpw = &op->oq_pwdexop;

		params = json_object();
		err = json_object_set_new( params, "userID", json_stringbv( &op->o_req_dn ) );
		err = json_object_set_new( params, "oldPassword", json_stringbv( &qpw->rs_old ) );
		err = json_object_set_new( params, "newPassword", json_stringbv( &qpw->rs_new ) );
	} else {
		/* generic extended operation */
	}

	if( si->si_cookie ) {
		json_object_set( params, "cookie", si->si_cookie );
	}

	err = json_object_add_suffixes( params, op->o_bd );
	err = json_object_add_conn( params, op->o_conn, si );

	json_request = json_pack( "{s:s,s:s,s:o,s:I}",
		"jsonrpc", "2.0",
		"method", "ldap.ext.passwordModify",
		"params", params,
		"id", (json_int_t) op->o_msgid
	);
	if( !json_request ) {
		fprintf( stderr, "ERR: %s\n", error.text );
	}

	err = json_dumpf( json_request, fp, 0 );
	json_decref( json_request );
	fprintf( fp, "\n" );
	fflush( fp );

	result = json_loadf( fp, 0, &error );
	if( !result ) {
		fprintf( stderr, "Error: %s\n", error.text );
	}

	/* read in the result and send it along */
	sock_read_and_send_results( op, rs, result );

	fclose( fp );
	return( 0 );
}