#include "dmap_parser.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum {
	DMAP_UNKNOWN,
	DMAP_INT,
	DMAP_STR,
	DMAP_DATE,
	DMAP_VERS,
	DMAP_DICT
} DMAP_FIELD;

typedef struct {
	const char *code;
	DMAP_FIELD type;
	const char *name;
} dmap_type;

static dmap_type dmap_types[] = {
	{ "abal", DMAP_DICT, "daap.browsealbumlisting" },
	{ "abar", DMAP_DICT, "daap.browseartistlisting" },
	{ "abcp", DMAP_DICT, "daap.browsecomposerlisting" },
	{ "abgn", DMAP_DICT, "daap.browsegenrelisting" },
	{ "abpl", DMAP_INT,  "daap.baseplaylist" },
	{ "abro", DMAP_DICT, "daap.databasebrowse" },
	{ "adbs", DMAP_DICT, "daap.databasesongs" },
	{ "aeAD", DMAP_DICT, "com.apple.itunes.adam.ids.array" },
	{ "aeAI", DMAP_INT,  "com.apple.itunes.itms.artistid" },
	{ "aeCI", DMAP_INT,  "com.apple.itunes.itms.composerid" },
	{ "aeCR", DMAP_STR,  "com.apple.itunes.content.rating" } ,
	{ "aeDP", DMAP_INT,  "com.apple.itunes.drm.platform.id" },
	{ "aeDR", DMAP_INT,  "com.apple.itunes.drm.user.id" },
	{ "aeDV", DMAP_INT,  "com.apple.itunes.drm.versions" },
	{ "aeEN", DMAP_STR,  "com.apple.itunes.episode.num.str" },
	{ "aeES", DMAP_INT,  "com.apple.itunes.episode.sort" },
	{ "aeGD", DMAP_INT,  "com.apple.itunes.gapless.enc.dr" } ,
	{ "aeGE", DMAP_INT,  "com.apple.itunes.gapless.enc.del" },
	{ "aeGH", DMAP_INT,  "com.apple.itunes.gapless.heur" },
	{ "aeGI", DMAP_INT,  "com.apple.itunes.itms.genreid" },
	{ "aeGR", DMAP_INT,  "com.apple.itunes.gapless.resy" },
	{ "aeGU", DMAP_INT,  "com.apple.itunes.gapless.dur" },
	{ "aeHD", DMAP_INT,  "com.apple.itunes.is.hd.video" },
	{ "aeHV", DMAP_INT,  "com.apple.itunes.has.video" },
	{ "aeK1", DMAP_INT,  "com.apple.itunes.drm.key1.id" },
	{ "aeK2", DMAP_INT,  "com.apple.itunes.drm.key2.id" },
	{ "aeMK", DMAP_INT,  "com.apple.itunes.mediakind" },
	{ "aeMk", DMAP_INT,  "com.apple.itunes.extended.media.kind" },
	{ "aeND", DMAP_INT,  "com.apple.itunes.non.drm.user.id" },
	{ "aeNN", DMAP_STR,  "com.apple.itunes.network.name" },
	{ "aeNV", DMAP_INT,  "com.apple.itunes.norm.volume" },
	{ "aePC", DMAP_INT,  "com.apple.itunes.is.podcast" },
	{ "aePI", DMAP_INT,  "com.apple.itunes.itms.playlistid" },
	{ "aePP", DMAP_INT,  "com.apple.itunes.is.podcast.playlist" },
	{ "aePS", DMAP_INT,  "com.apple.itunes.special.playlist" },
	{ "aeSE", DMAP_INT,  "com.apple.itunes.store.pers.id" },
	{ "aeSF", DMAP_INT,  "com.apple.itunes.itms.storefrontid" },
	{ "aeSG", DMAP_INT,  "com.apple.itunes.saved.genius" },
	{ "aeSI", DMAP_INT,  "com.apple.itunes.itms.songid" },
	{ "aeSN", DMAP_STR,  "com.apple.itunes.series.name" },
	{ "aeSP", DMAP_INT,  "com.apple.itunes.smart.playlist" },
	{ "aeSU", DMAP_INT,  "com.apple.itunes.season.num" },
	{ "aeSV", DMAP_INT,  "com.apple.itunes.music.sharing.version" },
	{ "aeXD", DMAP_STR,  "com.apple.itunes.xid" },
	{ "agrp", DMAP_STR,  "daap.songgrouping" },
	{ "aply", DMAP_DICT, "daap.databaseplaylists" },
	{ "aprm", DMAP_INT,  "daap.playlistrepeatmode" },
	{ "apro", DMAP_VERS, "daap.protocolversion" },
	{ "apsm", DMAP_INT,  "daap.playlistshufflemode" },
	{ "apso", DMAP_DICT, "daap.playlistsongs" },
	{ "arif", DMAP_DICT, "daap.resolveinfo" },
	{ "arsv", DMAP_DICT, "daap.resolve" },
	{ "asaa", DMAP_STR,  "daap.songalbumartist" },
	{ "asai", DMAP_INT,  "daap.songalbumid" },
	{ "asal", DMAP_STR,  "daap.songalbum" },
	{ "asar", DMAP_STR,  "daap.songartist" },
	{ "asbk", DMAP_INT,  "daap.bookmarkable" },
	{ "asbo", DMAP_INT,  "daap.songbookmark" },
	{ "asbr", DMAP_INT,  "daap.songbitrate" },
	{ "asbt", DMAP_INT,  "daap.songbeatsperminute" },
	{ "ascd", DMAP_INT,  "daap.songcodectype" },
	{ "ascm", DMAP_STR,  "daap.songcomment" },
	{ "ascn", DMAP_STR,  "daap.songcontentdescription" },
	{ "asco", DMAP_INT,  "daap.songcompilation" },
	{ "ascp", DMAP_STR,  "daap.songcomposer" },
	{ "ascr", DMAP_INT,  "daap.songcontentrating" },
	{ "ascs", DMAP_INT,  "daap.songcodecsubtype" },
	{ "asct", DMAP_STR,  "daap.songcategory" },
	{ "asda", DMAP_DATE, "daap.songdateadded" },
	{ "asdb", DMAP_INT,  "daap.songdisabled" },
	{ "asdc", DMAP_INT,  "daap.songdisccount" },
	{ "asdk", DMAP_INT,  "daap.songdatakind" },
	{ "asdm", DMAP_DATE, "daap.songdatemodified" },
	{ "asdn", DMAP_INT,  "daap.songdiscnumber" },
	{ "asdp", DMAP_DATE, "daap.songdatepurchased" },
	{ "asdr", DMAP_DATE, "daap.songdatereleased" },
	{ "asdt", DMAP_STR,  "daap.songdescription" },
	{ "ased", DMAP_INT,  "daap.songextradata" },
	{ "aseq", DMAP_STR,  "daap.songeqpreset" },
	{ "asfm", DMAP_STR,  "daap.songformat" },
	{ "asgn", DMAP_STR,  "daap.songgenre" },
	{ "asgp", DMAP_INT,  "daap.songgapless" },
	{ "ashp", DMAP_INT,  "daap.songhasbeenplayed" },
	{ "asky", DMAP_STR,  "daap.songkeywords" },
	{ "aslc", DMAP_STR,  "daap.songlongcontentdescription" },
	{ "asls", DMAP_INT,  "daap.songlongsize" },
	{ "aspu", DMAP_STR,  "daap.songpodcasturl" },
	{ "asrv", DMAP_INT,  "daap.songrelativevolume" },
	{ "assa", DMAP_STR,  "daap.sortartist" },
	{ "assc", DMAP_STR,  "daap.sortcomposer" },
	{ "assl", DMAP_STR,  "daap.sortalbumartist" },
	{ "assn", DMAP_STR,  "daap.sortname" },
	{ "assp", DMAP_INT,  "daap.songstoptime" },
	{ "assr", DMAP_INT,  "daap.songsamplerate" },
	{ "asss", DMAP_STR,  "daap.sortseriesname" },
	{ "asst", DMAP_INT,  "daap.songstarttime" },
	{ "assu", DMAP_STR,  "daap.sortalbum" },
	{ "assz", DMAP_INT,  "daap.songsize" },
	{ "astc", DMAP_INT,  "daap.songtrackcount" },
	{ "astm", DMAP_INT,  "daap.songtime" },
	{ "astn", DMAP_INT,  "daap.songtracknumber" },
	{ "asul", DMAP_STR,  "daap.songdataurl" },
	{ "asur", DMAP_INT,  "daap.songuserrating" },
	{ "asyr", DMAP_INT,  "daap.songyear" },
	{ "ated", DMAP_INT,  "daap.supportsextradata" },
	{ "avdb", DMAP_DICT, "daap.serverdatabases" },
	{ "caar", DMAP_INT,  "dacp.availablerepeatstates" }, /* some kind of ORed list? 1=disabled, 6=repeat all & repeat one enabled */
	{ "caas", DMAP_INT,  "dacp.availablshufflestates" }, /* some kind of ORed list? 1=disabled, 2=enabled */
	{ "caci", DMAP_DICT, "caci" },
	{ "cafe", DMAP_INT,  "dacp.fullscreenenabled" },
	{ "cafs", DMAP_INT,  "dacp.fullscreen" },
	{ "caia", DMAP_INT,  "dacp.isactive" },
	{ "cana", DMAP_STR,  "dacp.nowplayingartist" },
	{ "cang", DMAP_STR,  "dacp.nowplayinggenre" },
	{ "canl", DMAP_STR,  "dacp.nowplayingalbum" },
	{ "cann", DMAP_STR,  "dacp.nowplayingname" },
	{ "canp", DMAP_INT,  "dacp.nowplayingids" },
	{ "caps", DMAP_INT,  "dacp.playerstate" },
	{ "carp", DMAP_INT,  "dacp.repeatstate" },
	{ "cash", DMAP_INT,  "dacp.shufflestate" },
	{ "casp", DMAP_DICT, "dacp.speakers" },
	{ "cavc", DMAP_INT,  "dmcp.volumecontrollable" }, /* Should this be cavc? Was cmvc */
	{ "cave", DMAP_INT,  "dacp.visualizerenabled" },
	{ "cavs", DMAP_INT,  "dacp.visualizer" },
	{ "ceJC", DMAP_INT,  "com.apple.itunes.jukebox.client.vote" },
	{ "ceJI", DMAP_INT,  "com.apple.itunes.jukebox.current" },
	{ "ceJS", DMAP_INT,  "com.apple.itunes.jukebox.score" },
	{ "ceJV", DMAP_INT,  "com.apple.itunes.jukebox.vote" },
	{ "cmgt", DMAP_DICT, "dmcp.getpropertyresponse" },
	{ "cmmk", DMAP_INT,  "dmcp.mediakind" },
	{ "cmsr", DMAP_INT,  "dmcp.serverrevision" },
	{ "cmst", DMAP_DICT, "dmcp.playstatus" },
	{ "cmvo", DMAP_INT,  "dmcp.volume" },
	{ "mbcl", DMAP_DICT, "dmap.bag" },
	{ "mccr", DMAP_DICT, "dmap.contentcodesresponse" },
	{ "mcna", DMAP_STR,  "dmap.contentcodesname" },
	{ "mcnm", DMAP_INT,  "dmap.contentcodesnumber" },
	{ "mcon", DMAP_DICT, "dmap.container" },
	{ "mctc", DMAP_INT,  "dmap.containercount" },
	{ "mcti", DMAP_INT,  "dmap.containeritemid" },
	{ "mcty", DMAP_INT,  "dmap.contentcodestype" },
	{ "mdcl", DMAP_DICT, "dmap.dictionary" },
	{ "meds", DMAP_INT,  "dmap.editcommandssupported" },
	{ "miid", DMAP_INT,  "dmap.itemid" },
	{ "mikd", DMAP_INT,  "dmap.itemkind" },
	{ "mimc", DMAP_INT,  "dmap.itemcount" },
	{ "minm", DMAP_STR,  "dmap.itemname" },
	{ "minm", DMAP_STR,  "dmap.itemname" },
	{ "mlcl", DMAP_DICT, "dmap.listing" },
	{ "mlid", DMAP_INT,  "dmap.sessionid" },
	{ "mlit", DMAP_DICT, "dmap.listingitem" },
	{ "mlog", DMAP_DICT, "dmap.loginresponse" },
	{ "mpco", DMAP_INT,  "dmap.parentcontainerid" },
	{ "mper", DMAP_INT,  "dmap.persistentid" },
	{ "mpro", DMAP_VERS, "dmap.protocolversion" },
	{ "mrco", DMAP_INT,  "dmap.returnedcount" },
	{ "msal", DMAP_INT,  "dmap.supportsautologout" },
	{ "msas", DMAP_INT,  "dmap.authenticationschemes" },
	{ "msau", DMAP_INT,  "dmap.authenticationmethod" },
	{ "msbr", DMAP_INT,  "dmap.supportsbrowse" },
	{ "msdc", DMAP_INT,  "dmap.databasescount" },
	{ "msex", DMAP_INT,  "dmap.supportsextensions" },
	{ "msix", DMAP_INT,  "dmap.supportsindex" },
	{ "mslr", DMAP_INT,  "dmap.loginrequired" },
	{ "msma", DMAP_INT,  "dmap.machineaddress" },
	{ "msml", DMAP_DICT, "msml" },
	{ "mspi", DMAP_INT,  "dmap.supportspersistentids" },
	{ "msqy", DMAP_INT,  "dmap.supportsquery" },
	{ "msrs", DMAP_INT,  "dmap.supportsresolve" },
	{ "msrv", DMAP_DICT, "dmap.serverinforesponse" },
	{ "mstc", DMAP_DATE, "dmap.utctime" },
	{ "mstm", DMAP_INT,  "dmap.timeoutinterval" },
	{ "msto", DMAP_INT,  "dmap.utcoffset" },
	{ "msts", DMAP_STR,  "dmap.statusstring" },
	{ "mstt", DMAP_INT,  "dmap.status" },
	{ "msup", DMAP_INT,  "dmap.supportsupdate" },
	{ "mtco", DMAP_INT,  "dmap.specifiedtotalcount" },
	{ "mudl", DMAP_DICT, "dmap.deletedidlisting" },
	{ "mupd", DMAP_DICT, "dmap.updateresponse" },
	{ "musr", DMAP_INT,  "dmap.serverrevision" },
	{ "muty", DMAP_INT,  "dmap.updatetype" }
};
static int dmap_type_count = sizeof(dmap_types) / sizeof(dmap_type);

int dmap_type_sort(const void *c1, const void *c2) {
	dmap_type *t1 = (dmap_type *)c1;
	dmap_type *t2 = (dmap_type *)c2;
	return strncmp(t1->code, t2->code, 4);
}

dmap_type *dmap_type_from_code(const char *code) {
	dmap_type key;
	key.code = code;
	return bsearch(&key, dmap_types, dmap_type_count, sizeof(dmap_type), dmap_type_sort);
}

const char *dmap_name_from_code(const char *code) {
	dmap_type *t = dmap_type_from_code(code);
	return t != 0 ? t->name : 0;
}

int16_t dmap_read_i16(const char *buf)
{
	return ((buf[0] & 0xff) <<  8) |
	((buf[1] & 0xff));
}

int32_t dmap_read_i32(const char *buf)
{
	return ((buf[0] & 0xff) << 24) |
	((buf[1] & 0xff) << 16) |
	((buf[2] & 0xff) <<  8) |
	((buf[3] & 0xff));
}

int64_t dmap_read_i64(const char *buf)
{
	return ((int64_t)(buf[0] & 0xff) << 56) |
	((int64_t)(buf[1] & 0xff) << 48) |
	((int64_t)(buf[2] & 0xff) << 40) |
	((int64_t)(buf[3] & 0xff) << 32) |
	((int64_t)(buf[4] & 0xff) << 24) |
	((int64_t)(buf[5] & 0xff) << 16) |
	((int64_t)(buf[6] & 0xff) <<  8) |
	((int64_t)(buf[7] & 0xff));
}

int dmap_is_codechar(const char c) {
	return (c > 'A' && c < 'Z') || (c > 'a' && c < 'z');
}

int dmap_parse(const dmap_settings *settings, const char *buf, int len) {
	dmap_type *t;
	DMAP_FIELD field_type;
	int field_len;
	const char *field_name;
	const char *p = buf;
	const char *end = buf + len;
	char code[5];
	code[4] = '\0';

	while (end - p >= 8) {
		strncpy(code, p, 4);
		t = dmap_type_from_code(code);
		p += 4;

		field_len = dmap_read_i32(p);
		p += 4;

		if (field_len < 0 || p + field_len > end)
			return -1;

		if (t) {
			field_type = t->type;
			field_name = t->name;
		} else {
			/* Make a best guess of the type */
			field_type = DMAP_UNKNOWN;
			field_name = code;

			if (field_len >= 8) {
				/* Look for a four char code followed by a length within the current field */
				if (dmap_is_codechar(p[0]) && dmap_is_codechar(p[1]) && dmap_is_codechar(p[2]) && dmap_is_codechar(p[3])) {
					if (dmap_read_i32(p + 4) < field_len)
						field_type = DMAP_DICT;
				}
			}

			if (field_type == DMAP_UNKNOWN) {
				int i, is_string = 1;
				for (i=0; i < field_len; i++) {
					if (!isascii(p[i]) || p[i] < 2) {
						is_string = 0;
						break;
					}
				}

				field_type = is_string ? DMAP_STR : DMAP_INT;
			}
		}

		switch (field_type) {
			case DMAP_INT:
				/* Determine the integer's type based on its size */
				switch (field_len) {
					case 1:
						/* TODO: Treat as char? */
						if (settings->on_int32)
							settings->on_int32(settings->ctx, code, field_name, *p);
						break;
					case 2:
						if (settings->on_int32)
							settings->on_int32(settings->ctx, code, field_name, dmap_read_i16(p));
						break;
					case 4:
						if (settings->on_int32)
							settings->on_int32(settings->ctx, code, field_name, dmap_read_i32(p));
						break;
					case 8:
						if (settings->on_int64)
							settings->on_int64(settings->ctx, code, field_name, dmap_read_i64(p));
						break;
					default:
						if (settings->on_data)
							settings->on_data(settings->ctx, code, field_name, p, field_len);
						break;
				}
				break;
			case DMAP_STR:
				if (settings->on_string)
					settings->on_string(settings->ctx, code, field_name, p, field_len);
				break;
			case DMAP_DICT:
				if (settings->on_dict_start)
					settings->on_dict_start(settings->ctx, code, field_name);
				dmap_parse(settings, p, field_len);
				if (settings->on_dict_end)
					settings->on_dict_end(settings->ctx, code, field_name);
				break;
		}

		p += field_len;
	}

	return 0;
}
