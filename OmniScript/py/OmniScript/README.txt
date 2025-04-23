=========
OmniTools
=========

OmniTools provides an interface, omniscript, to Savvius Engines.
You might find it useful for tasks involving captures and packets.
Typical usage often looks like this::

    #!/usr/bin/env python

    from omniscript import OmniScript

    omni = omniscript.OmniScript()
    engine = omni.create_engine('localhost')
	engine.connect('User', 'password')
    capture = engine.findCapture('User - Capture 1')

- - - - - Remove - - - - -
Formatting information follows.

*Italics*, **bold**, and ``monospace`` look like this.

A Section
=========

Lists look like this:

* First

* Second. Can be multiple lines
  but must be indented properly.

A Sub-Section
-------------

Numbered lists look like you'd expect:

1. hi there

2. must be going

Urls are http://like.this and links can be
written `like this <http://www.example.com/foo/bar>`_.
