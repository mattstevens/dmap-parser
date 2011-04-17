#include "dmap_parser.h"
#include <stdint.h>

enum DMAP_FIELD {
	DMAP_BYTE = 1,
	DMAP_SHORT = 3,
	DMAP_USHORT,
	DMAP_INT,
	DMAP_UINT,
	DMAP_LONG,
	DMAP_ULONG,
	DMAP_STR,
	DMAP_DATE,
	DMAP_VERS,
	DMAP_DICT
};

typedef struct dmap_type {
	const uint32_t code;
	const char type;
	const char* name;
} dmap_type;

static dmap_type dmap_types[] = {
	{ 'abal', DMAP_DICT,   "daap.browsealbumlisting" },
	{ 'abar', DMAP_DICT,   "daap.browseartistlisting" },
	{ 'abcp', DMAP_DICT,   "daap.browsecomposerlisting" },
	{ 'abgn', DMAP_DICT,   "daap.browsegenrelisting" },
	{ 'abpl', DMAP_BYTE,   "daap.baseplaylist" },
	{ 'abro', DMAP_DICT,   "daap.databasebrowse" },
	{ 'adbs', DMAP_DICT,   "daap.databasesongs" },
	{ 'aeAD', DMAP_DICT,   "com.apple.itunes.adam.ids.array" },
	{ 'aeAI', DMAP_INT,    "com.apple.itunes.itms.artistid" },
	{ 'aeCI', DMAP_INT,    "com.apple.itunes.itms.composerid" },
	{ 'aeCR', DMAP_STR,    "com.apple.itunes.content.rating" },
	{ 'aeDP', DMAP_INT,    "com.apple.itunes.drm.platform.id" },
	{ 'aeDR', DMAP_LONG,   "com.apple.itunes.drm.user.id" },
	{ 'aeDV', DMAP_INT,    "com.apple.itunes.drm.versions" },
	{ 'aeEN', DMAP_STR,    "com.apple.itunes.episode.num.str" },
	{ 'aeES', DMAP_INT,    "com.apple.itunes.episode.sort" },
	{ 'aeGD', DMAP_INT,    "com.apple.itunes.gapless.enc.dr" },
	{ 'aeGE', DMAP_INT,    "com.apple.itunes.gapless.enc.del" },
	{ 'aeGH', DMAP_INT,    "com.apple.itunes.gapless.heur" },
	{ 'aeGI', DMAP_INT,    "com.apple.itunes.itms.genreid" },
	{ 'aeGR', DMAP_LONG,   "com.apple.itunes.gapless.resy" },
	{ 'aeGU', DMAP_LONG,   "com.apple.itunes.gapless.dur" },
	{ 'aeHD', DMAP_BYTE,   "com.apple.itunes.is.hd.video" },
	{ 'aeHV', DMAP_BYTE,   "com.apple.itunes.has.video" },
	{ 'aeK1', DMAP_LONG,   "com.apple.itunes.drm.key1.id" },
	{ 'aeK2', DMAP_LONG,   "com.apple.itunes.drm.key2.id" },
	{ 'aeMK', DMAP_BYTE,   "com.apple.itunes.mediakind" },
	{ 'aeMk', DMAP_INT,    "com.apple.itunes.extended.media.kind" },
	{ 'aeND', DMAP_LONG,   "com.apple.itunes.non.drm.user.id" },
	{ 'aeNN', DMAP_STR,    "com.apple.itunes.network.name" },
	{ 'aeNV', DMAP_INT,    "com.apple.itunes.norm.volume" },
	{ 'aePC', DMAP_BYTE,   "com.apple.itunes.is.podcast" },
	{ 'aePI', DMAP_INT,    "com.apple.itunes.itms.playlistid" },
	{ 'aePP', DMAP_BYTE,   "com.apple.itunes.is.podcast.playlist" },
	{ 'aePS', DMAP_BYTE,   "com.apple.itunes.special.playlist" },
	{ 'aeSE', DMAP_LONG,   "com.apple.itunes.store.pers.id" },
	{ 'aeSF', DMAP_INT,    "com.apple.itunes.itms.storefrontid" },
	{ 'aeSG', DMAP_BYTE,   "com.apple.itunes.saved.genius" },
	{ 'aeSI', DMAP_INT,    "com.apple.itunes.itms.songid" },
	{ 'aeSN', DMAP_STR,    "com.apple.itunes.series.name" },
	{ 'aeSP', DMAP_BYTE,   "com.apple.itunes.smart.playlist" },
	{ 'aeSU', DMAP_INT,    "com.apple.itunes.season.num" },
	{ 'aeSV', DMAP_INT,    "com.apple.itunes.music.sharing.version" },
	{ 'aeXD', DMAP_STR,    "com.apple.itunes.xid" },
	{ 'agrp', DMAP_STR,    "daap.songgrouping" },
	{ 'aply', DMAP_DICT,   "daap.databaseplaylists" },
	{ 'aprm', DMAP_BYTE,   "daap.playlistrepeatmode" },
	{ 'apro', DMAP_VERS,   "daap.protocolversion" },
	{ 'apsm', DMAP_BYTE,   "daap.playlistshufflemode" },
	{ 'apso', DMAP_DICT,   "daap.playlistsongs" },
	{ 'arif', DMAP_DICT,   "daap.resolveinfo" },
	{ 'arsv', DMAP_DICT,   "daap.resolve" },
	{ 'asaa', DMAP_STR,    "daap.songalbumartist" },
	{ 'asai', DMAP_LONG,   "daap.songalbumid" },
	{ 'asal', DMAP_STR,    "daap.songalbum" },
	{ 'asar', DMAP_STR,    "daap.songartist" },
	{ 'asbk', DMAP_BYTE,   "daap.bookmarkable" },
	{ 'asbo', DMAP_INT,    "daap.songbookmark" },
	{ 'asbr', DMAP_SHORT,  "daap.songbitrate" },
	{ 'asbt', DMAP_SHORT,  "daap.songbeatsperminute" },
	{ 'ascd', DMAP_INT,    "daap.songcodectype" },
	{ 'ascm', DMAP_STR,    "daap.songcomment" },
	{ 'ascn', DMAP_STR,    "daap.songcontentdescription" },
	{ 'asco', DMAP_BYTE,   "daap.songcompilation" },
	{ 'ascp', DMAP_STR,    "daap.songcomposer" },
	{ 'ascr', DMAP_BYTE,   "daap.songcontentrating" },
	{ 'ascs', DMAP_INT,    "daap.songcodecsubtype" },
	{ 'asct', DMAP_STR,    "daap.songcategory" },
	{ 'asda', DMAP_DATE,   "daap.songdateadded" },
	{ 'asdb', DMAP_BYTE,   "daap.songdisabled" },
	{ 'asdc', DMAP_SHORT,  "daap.songdisccount" },
	{ 'asdk', DMAP_BYTE,   "daap.songdatakind" },
	{ 'asdm', DMAP_DATE,   "daap.songdatemodified" },
	{ 'asdn', DMAP_SHORT,  "daap.songdiscnumber" },
	{ 'asdp', DMAP_DATE,   "daap.songdatepurchased" },
	{ 'asdr', DMAP_DATE,   "daap.songdatereleased" },
	{ 'asdt', DMAP_STR,    "daap.songdescription" },
	{ 'ased', DMAP_SHORT,  "daap.songextradata" },
	{ 'aseq', DMAP_STR,    "daap.songeqpreset" },
	{ 'asfm', DMAP_STR,    "daap.songformat" },
	{ 'asgn', DMAP_STR,    "daap.songgenre" },
	{ 'asgp', DMAP_BYTE,   "daap.songgapless" },
	{ 'ashp', DMAP_BYTE,   "daap.songhasbeenplayed" },
	{ 'asky', DMAP_STR,    "daap.songkeywords" },
	{ 'aslc', DMAP_STR,    "daap.songlongcontentdescription" },
	{ 'asls', DMAP_LONG,   "daap.songlongsize" },
	{ 'aspu', DMAP_STR,    "daap.songpodcasturl" },
	{ 'asrv', DMAP_BYTE,   "daap.songrelativevolume" },
	{ 'assa', DMAP_STR,    "daap.sortartist" },
	{ 'assc', DMAP_STR,    "daap.sortcomposer" },
	{ 'assl', DMAP_STR,    "daap.sortalbumartist" },
	{ 'assn', DMAP_STR,    "daap.sortname" },
	{ 'assp', DMAP_INT,    "daap.songstoptime" },
	{ 'assr', DMAP_INT,    "daap.songsamplerate" },
	{ 'asss', DMAP_STR,    "daap.sortseriesname" },
	{ 'asst', DMAP_INT,    "daap.songstarttime" },
	{ 'assu', DMAP_STR,    "daap.sortalbum" },
	{ 'assz', DMAP_INT,    "daap.songsize" },
	{ 'astc', DMAP_SHORT,  "daap.songtrackcount" },
	{ 'astm', DMAP_INT,    "daap.songtime" },
	{ 'astn', DMAP_SHORT,  "daap.songtracknumber" },
	{ 'asul', DMAP_STR,    "daap.songdataurl" },
	{ 'asur', DMAP_BYTE,   "daap.songuserrating" },
	{ 'asyr', DMAP_SHORT,  "daap.songyear" },
	{ 'ated', DMAP_SHORT,  "daap.supportsextradata" },
	{ 'avdb', DMAP_DICT,   "daap.serverdatabases" },
	{ 'caar', DMAP_INT,    "dacp.availablerepeatstates" }, /* some kind of ORed list? 1=disabled, 6=repeat all & repeat one enabled */
	{ 'caas', DMAP_INT,    "dacp.availablshufflestates" }, /* some kind of ORed list? 1=disabled, 2=enabled */
	{ 'caci', DMAP_DICT,   "caci" },
	{ 'cana', DMAP_STR,    "dacp.nowplayingartist" },
	{ 'cang', DMAP_STR,    "dacp.nowplayinggenre" },
	{ 'canl', DMAP_STR,    "dacp.nowplayingalbum" },
	{ 'cann', DMAP_STR,    "dacp.nowplayingname" },
	{ 'caps', DMAP_BYTE,   "dacp.playerstate" },
	{ 'carp', DMAP_BYTE,   "dacp.repeatstate" },
	{ 'cash', DMAP_BYTE,   "dacp.shufflestate" },
	{ 'casp', DMAP_DICT,   "dacp.speakers" },
	{ 'cavc', DMAP_BYTE,   "dmcp.volumecontrollable" }, /* Should this be cavc? Was cmvc */
	{ 'ceJC', DMAP_BYTE,   "com.apple.itunes.jukebox.client.vote" },
	{ 'ceJI', DMAP_INT,    "com.apple.itunes.jukebox.current" },
	{ 'ceJS', DMAP_USHORT, "com.apple.itunes.jukebox.score" },
	{ 'ceJV', DMAP_INT,    "com.apple.itunes.jukebox.vote" },
	{ 'cmgt', DMAP_DICT,   "dmcp.getpropertyresponse" },
	{ 'cmsr', DMAP_INT,    "dmcp.revisionid" },
	{ 'cmst', DMAP_DICT,   "dmcp.playstatus" },
	{ 'cmvo', DMAP_INT,    "dmcp.volume" },
	{ 'mbcl', DMAP_DICT,   "dmap.bag" },
	{ 'mccr', DMAP_DICT,   "dmap.contentcodesresponse" },
	{ 'mcna', DMAP_STR,    "dmap.contentcodesname" },
	{ 'mcnm', DMAP_INT,    "dmap.contentcodesnumber" },
	{ 'mcon', DMAP_DICT,   "dmap.container" },
	{ 'mctc', DMAP_INT,    "dmap.containercount" },
	{ 'mcti', DMAP_INT,    "dmap.containeritemid" },
	{ 'mcty', DMAP_SHORT,  "dmap.contentcodestype" },
	{ 'mdcl', DMAP_DICT,   "dmap.dictionary" },
	{ 'meds', DMAP_INT,    "dmap.editcommandssupported" },
	{ 'miid', DMAP_INT,    "dmap.itemid" },
	{ 'mikd', DMAP_BYTE,   "dmap.itemkind" },
	{ 'mimc', DMAP_INT,    "dmap.itemcount" },
	{ 'minm', DMAP_STR,    "dmap.itemname" },
	{ 'minm', DMAP_STR,    "dmap.itemname" },
	{ 'mlcl', DMAP_DICT,   "dmap.listing" },
	{ 'mlid', DMAP_INT,    "dmap.sessionid" },
	{ 'mlit', DMAP_DICT,   "dmap.listingitem" },
	{ 'mlog', DMAP_DICT,   "dmap.loginresponse" },
	{ 'mpco', DMAP_INT,    "dmap.parentcontainerid" },
	{ 'mper', DMAP_LONG,   "dmap.persistentid" },
	{ 'mpro', DMAP_VERS,   "dmap.protocolversion" },
	{ 'mrco', DMAP_INT,    "dmap.returnedcount" },
	{ 'msal', DMAP_BYTE,   "dmap.supportsautologout" },
	{ 'msas', DMAP_INT,    "dmap.authenticationschemes" },
	{ 'msau', DMAP_BYTE,   "dmap.authenticationmethod" },
	{ 'msbr', DMAP_BYTE,   "dmap.supportsbrowse" },
	{ 'msdc', DMAP_INT,    "dmap.databasescount" },
	{ 'msex', DMAP_BYTE,   "dmap.supportsextensions" },
	{ 'msix', DMAP_BYTE,   "dmap.supportsindex" },
	{ 'mslr', DMAP_BYTE,   "dmap.loginrequired" },
	{ 'msma', DMAP_LONG,   "dmap.macaddress" },
	{ 'msml', DMAP_DICT,   "msml" },
	{ 'mspi', DMAP_BYTE,   "dmap.supportspersistentids" },
	{ 'msqy', DMAP_BYTE,   "dmap.supportsquery" },
	{ 'msrs', DMAP_BYTE,   "dmap.supportsresolve" },
	{ 'msrv', DMAP_DICT,   "dmap.serverinforesponse" },
	{ 'mstc', DMAP_DATE,   "dmap.utctime" },
	{ 'mstm', DMAP_INT,    "dmap.timeoutinterval" },
	{ 'msto', DMAP_UINT,   "dmap.utcoffset" },
	{ 'msts', DMAP_STR,    "dmap.statusstring" },
	{ 'mstt', DMAP_INT,    "dmap.status" },
	{ 'msup', DMAP_BYTE,   "dmap.supportsupdate" },
	{ 'mtco', DMAP_INT,    "dmap.specifiedtotalcount" },
	{ 'mudl', DMAP_DICT,   "dmap.deletedidlisting" },
	{ 'mupd', DMAP_DICT,   "dmap.updateresponse" },
	{ 'musr', DMAP_INT,    "dmap.serverrevision" },
	{ 'muty', DMAP_BYTE,   "dmap.updatetype" },
	{ 0, 0, 0 }
};

dmap_type* dmap_type_from_code(uint32_t code) {
	dmap_type* t = dmap_types;
	while (t->code != 0) {
		if (t->code == code) {
			return t;
		}
		t++;
	}

	return 0;
}

int32_t dmap_read_i32(const char *buf)
{
	return ((buf[0] & 0xff) << 24) |
    ((buf[1] & 0xff) << 16) |
    ((buf[2] & 0xff) <<  8) |
    ((buf[3] & 0xff));
}

int dmap_parse(const char* buf, int len) {
	uint32_t code = 0;
	uint32_t field_len = 0;
	char field_type;
	dmap_type *t = 0;
	const char *p = buf;
	const char *end = buf + len;

	while (end - p >= 8) {
		code = dmap_read_i32(p);
		t = dmap_type_from_code(code);
		p += 4;

		field_len = dmap_read_i32(p);
		p += 4;

		if (!t) {
			// make a best guess of the type
		} else {
			field_type = t->type;
		}

		p += field_len;
	}

	return 0;
}
