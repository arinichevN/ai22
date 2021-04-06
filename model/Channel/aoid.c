#include "aoid.h"

static void channel_serveAoidRequestSelf(void *vself, Aoid *oid, void *vserver, int command){
	Channel *self = (Channel *) vself;
	Acpls *server = (Acpls *) vserver;
	switch(command){
		case CMD_AOID_START:
			channel_start(self);
			acpls_reset(server);
			return;
		case CMD_AOID_STOP:
			channel_stop(self);
			acpls_reset(server);
			return;
		CASE_AOID_GET_ACP_COMMAND_SUPPORTED(SS)
	}
	acpls_reset(server);
}

//static void channel_serveRequestNone(void *vself, Aoid *oid, void *vserver, int command) {
	//Acpls *server = (Acpls *) vserver;
	//switch(command){
		//CASE_AOID_GET_ACP_COMMAND_SUPPORTED(None)
	//}
	//acpls_reset(server);
//}

static void channel_serveAoidRequestGGS(void *vself, Aoid *oid, void *vserver, int command, void (*sendRamParam)(Aoid *, Acpls *, Channel *), void (*sendNvramParam)(Aoid *, Acpls *, const ChannelParam *, yn_t), int (*setNvramParam)(Acpls *, ChannelParam *)){
	Channel *self = (Channel *) vself;
	Acpls *server = (Acpls *) vserver;
	switch(command){
		case CMD_AOID_GET_RAM_VALUE:
			sendRamParam(oid, server, self);
			{printd("channel get r id: "); printd(self->id); printd(", master ind: "); Device *m = (Device *) self->master->self; printdln(m->ind);}
			return;
		case CMD_AOID_GET_NVRAM_VALUE:{
				printd("channel get nv id: "); printd(self->id); printd(", master ind: "); Device *m = (Device *) self->master->self; printdln(m->ind);
				yn_t success = NO;
				ChannelParam param;
				memset(&param, 0, sizeof param);
				if(self->master->getChannelParam(self->master->self, self->device_kind, &param)){
					success = YES;
				}
				sendNvramParam(oid, server, &param, success);
			}
			return;
		case CMD_AOID_SET_NVRAM_VALUE:{
				printd("channel set nv id: "); printd(self->id); printd(", master ind: "); Device *m = (Device *) self->master->self; printdln(m->ind);
				ChannelParam param;
				if(!self->master->getChannelParam(self->master->self, self->device_kind, &param)){
					acpls_reset(server);
					return;
				}
				if(!setNvramParam(server, &param)){
					acpls_reset(server);
					return;
				}
				if(channelParam_check(&param) == ERROR_NO){
					self->master->saveChannelParam(self->master->self, self->device_kind, &param);
				}
			}
			acpls_reset(server);
			return;
		CASE_AOID_GET_ACP_COMMAND_SUPPORTED(GGS)
	}
	acpls_reset(server);
}

static void channel_sendRamParamId(Aoid *oid, Acpls *server, Channel *channel){aoidServer_sendII(oid, server, channel->id, YES);}
static void channel_sendNvramParamId(Aoid *oid, Acpls *server, const ChannelParam *param, yn_t success){aoidServer_sendII(oid, server, param->id, success);}
static int channel_setNvramParamId(Acpls *server, ChannelParam *param){int v;	if(!acp_packGetCellI(server->acpl->buf, AOID_ACP_REQUEST_IND_PARAM1, &v)){return 0;} param->id = v;	return 1;}
static void channel_serveAoidRequestId(void *vself, Aoid *oid, void *vserver, int command){channel_serveAoidRequestGGS(vself, oid, vserver, command, channel_sendRamParamId, channel_sendNvramParamId, channel_setNvramParamId);}

static void channel_serveAoidRequestDeviceKind(void *vself, Aoid *oid, void *vserver, int command){
	Channel *self = (Channel *) vself;
	Acpls *server = (Acpls *) vserver;
	switch(command){
		case CMD_AOID_GET_RAM_VALUE:
			aoidServer_sendII(oid, server, self->device_kind, YES);
			return;
		case CMD_AOID_GET_NVRAM_VALUE:
			aoidServer_sendII(oid, server, self->device_kind, YES);
			return;
		CASE_AOID_GET_ACP_COMMAND_SUPPORTED(GG)
	}
	acpls_reset(server);
}

void channelAoid_build(ChannelAoid *self, Aoid *next, Aoid *parent, void *vchannel, size_t *id){
	//OBJE_ID_SET_PARAM(OID, 					NEXT,					PARENT,			KIND,							DESCR,							FUNCTION, 								DATA,		ID)
	AOID_SET_PARAM(&self->main,					&self->id,				parent,			AOID_KIND_COMPLEX,				AOID_DESCRIPTION_CHANNEL,		channel_serveAoidRequestSelf,			vchannel,	*id)
	AOID_SET_PARAM(&self->id,					&self->device_kind,		&self->main,	AOID_KIND_ID_PARAM,				AOID_DESCRIPTION_ID,			channel_serveAoidRequestId,				vchannel,	*id)
	AOID_SET_PARAM(&self->device_kind,			next,					&self->main,	AOID_KIND_DEVICE_KIND_PARAM,	AOID_DESCRIPTION_DEVICE_KIND,	channel_serveAoidRequestDeviceKind,		vchannel,	*id)
}
