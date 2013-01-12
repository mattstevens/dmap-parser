DMAP Parser
===========

A parser in C for the Digital Media Access Protocol (DMAP) and Digital Media Control Protocol (DMCP) used by iTunes. These are also referred to as the Digital Audio Access Protocol (DAAP) and Digital Audio Control Protocol (DACP).

Usage
-----

Populate a `dmap_settings` structure with the callbacks you're interested in and call `dmap_parse`. The parser maintains no state and expects to receive a complete message. Each callback returns both the four character code included in the message and a more human readable name for fields the parser is aware of.

Forward Compatibility
---------------------

The DMAP protocol does not encode any type information, requiring clients to know what type is used for each field ID and to skip unknown fields. However, through examination of the field data the parser is able to make a reasonable guess at the type. This is useful when examining new fields or messages.

dmapprint
---------

The dmapprint utility accepts DMAP input from stdin or a file and outputs a human readable representation of the message. For example:

	curl -H "Viewer-Only-Client: 1" http://127.0.0.1:3689/content-codes | dmapprint