DMAP Parser
===========

A parser in C for the Digital Media Access Protocol and its derivatives as used by iTunes and iPhoto. This includes support for the following protocols:

* Digital Media Access Protocol (DMAP)
	* Digital Audio Access Protocol (DAAP)
	* Digital Photo Access Protocol (DPAP)
* Digital Media Control Protocol (DMCP)
	* Digital Audio Control Protocol (DACP)

Usage
-----

Populate a `dmap_settings` structure with the callbacks you're interested in and call `dmap_parse`. The parser maintains no state and expects to receive a complete message. Each callback returns both the four character code included in the message and a more human readable name for fields the parser is aware of.

Forward Compatibility
---------------------

The DMAP protocol does not encode any type information, requiring clients to know what type is used for each field ID and to skip unknown fields. However, through examination of the field data the parser is able to make a reasonable guess at the type. This is useful when examining new fields or messages.

dmapprint
---------

The dmapprint utility accepts DMAP input from stdin or a file and outputs a human readable representation of the message. For example:

	> curl --silent --header "Viewer-Only-Client: 1" http://127.0.0.1:3689/content-codes | dmapprint
	dmap.contentcodesresponse:
	  dmap.status: 200
	  dmap.dictionary:
	    dmap.contentcodesnumber: miid
	    dmap.contentcodesname: dmap.itemid
	    dmap.contentcodestype: 5
	  dmap.dictionary:
	    dmap.contentcodesnumber: minm
	    dmap.contentcodesname: dmap.itemname
	    dmap.contentcodestype: 9
	    ...

This utility can be built by running `make dmapprint`.

Content Code Sources
--------------------

1. The /content-codes endpoints exposed by iTunes and iPhoto (see the dmapprint example above). The makefile includes a `checkcodes` target that parses the output of these endpoints and checks for new or updated codes.

2. Examination of network traffic between iTunes libraries and between iTunes and the Remote app. The /databases and /ctrl-int/1/getproperty endpoints are used to request specific properties by name and the results are returned using their fourchar equivalents. These mappings can be confirmed by requesting specific properties in isolation.

3. Output of `strings` on the iTunes binary. Since content code names use consistent prefixing this provides a good idea of the set of names that iTunes contains mappings for. These names can be mapped by querying them through the databases or getproperty endpoints. In a few cases these names are manually mapped where the name and fourchar follow a pattern in confirmed codes and the context in which they are used provides a high level of confidence in the mapping.
