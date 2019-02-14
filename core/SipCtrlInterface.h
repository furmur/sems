/*
 * $Id: SipCtrlInterface.h 1048 2008-07-15 18:48:07Z sayer $
 *
 * Copyright (C) 2007 Raphael Coeffic
 *
 * This file is part of SEMS, a free SIP media server.
 *
 * SEMS is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. This program is released under
 * the GPL with the additional exemption that compiling, linking,
 * and/or using OpenSSL is allowed.
 *
 * For a license to use the SEMS software under conditions
 * other than those described here, or to purchase support for this
 * software, please contact iptel.org by e-mail at the following addresses:
 *    info@iptel.org
 *
 * SEMS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef _SipCtrlInterface_h_
#define _SipCtrlInterface_h_

#include "sip/sip_ua.h"
#include "sip/sip_timers.h"
#include "AmThread.h"

#include <string>
#include <list>
#include "AmLCContainers.h"
using std::string;
using std::list;

class AmSipRequest;
class AmSipReply;

struct sip_msg;
struct sip_header;
class trans_ticket;


class udp_trsp_socket;
class udp_trsp;

class tcp_server_socket;
class tcp_trsp;

class tls_server_socket;
class tls_trsp;

class trsp_worker;

class _SipCtrlInterface:
    public sip_ua
{
    bool sip_msg2am_request(const sip_msg *msg, const trans_ticket& tt, AmSipRequest &request);
    bool sip_msg2am_reply(sip_msg *msg, AmSipReply &reply);
    
    void prepare_routes_uac(const list<sip_header*>& routes, string& route_field);
    void prepare_routes_uas(const list<sip_header*>& routes, string& route_field);

    friend class udp_trsp;

    AmCondition<bool> stopped;
    
    unsigned short    nr_udp_sockets;
    udp_trsp_socket** udp_sockets;

    unsigned short    nr_udp_servers;
    udp_trsp**        udp_servers;

    unsigned short    nr_tcp_sockets;
    tcp_server_socket** tcp_sockets;

    unsigned short    nr_tcp_servers;
    tcp_trsp**        tcp_servers;

    unsigned short    nr_tls_sockets;
    tls_server_socket** tls_sockets;

    unsigned short    nr_tls_servers;
    tls_trsp**        tls_servers;
    
    unsigned short    nr_trsp_workers;
    trsp_worker** trsp_workers;

    int alloc_udp_structs();
    int init_udp_servers(unsigned short if_num, unsigned short addr_num, SIP_info& info);

    int alloc_trsp_worker_structs();
    int init_trsp_workers();
    
    int alloc_tcp_structs();
    int init_tcp_servers(unsigned short if_num, unsigned short addr_num, SIP_info& info);
    
    int alloc_tls_structs();
    int init_tls_servers(unsigned short if_num, unsigned short addr_num, SIP_info& info);
public:

    static string outbound_host;
    static unsigned int outbound_port;
    static bool log_parsed_messages;
    static int udp_rcvbuf;

    _SipCtrlInterface();
    ~_SipCtrlInterface(){}

    int load();

    int run();
    void stop();
    void cleanup();

    /**
     * Sends a SIP request.
     *
     * @param req The request to send. If the request creates a transaction, 
     *            its ticket is written into req.tt.
     */
    static int send(AmSipRequest &req, const string& dialog_id,
		    const string& next_hop, int outbound_interface,
			unsigned int flags, sip_target_set* target_set_override,
            msg_logger* logger = NULL, msg_sensor *sensor = NULL,
			sip_timers_override *timers_override = NULL,
			int redirects_allowed = -1);

    /**
     * Sends a SIP reply. 
     *
     * @param rep The reply to be sent. 'rep.tt' should be set to transaction 
     *            ticket included in the SIP request.
     */
    static int send(const AmSipReply &rep, const string& dialog_id,
			msg_logger* logger = NULL, msg_sensor* sensor = NULL);

    /**
     * CANCELs an INVITE transaction.
     *
     * @param tt transaction ticket of the request to cancel.
     */
    static int cancel(trans_ticket* tt, const string& dialog_id,
		      unsigned int inv_cseq, const string& hdrs);

    /**
     * From sip_ua
     */
    void handle_sip_request(const trans_ticket& tt, sip_msg* msg);
    void handle_sip_reply(const trans_ticket& tt, const string& dialog_id, sip_msg* msg);
    void handle_reply_timeout(AmSipTimeoutEvent::EvType evt,
        sip_trans *tr, trans_bucket *buk=0);

    void getInfo(AmArg &ret);
};

typedef singleton<_SipCtrlInterface> SipCtrlInterface;

#endif

/** EMACS **
 * Local variables:
 * mode: c++
 * c-basic-offset: 4
 * End:
 */
