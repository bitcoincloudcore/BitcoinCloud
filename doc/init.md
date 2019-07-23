Sample init scripts and service configuration for bitcoincloudd
==========================================================

Sample scripts and configuration files for systemd, Upstart and OpenRC
can be found in the contrib/init folder.

    contrib/init/bitcoincloudd.service:    systemd service unit configuration
    contrib/init/bitcoincloudd.openrc:     OpenRC compatible SysV style init script
    contrib/init/bitcoincloudd.openrcconf: OpenRC conf.d file
    contrib/init/bitcoincloudd.conf:       Upstart service configuration file
    contrib/init/bitcoincloudd.init:       CentOS compatible SysV style init script

Service User
---------------------------------

All three Linux startup configurations assume the existence of a "bitcoincloud" user
and group.  They must be created before attempting to use these scripts.
The macOS configuration assumes bitcoincloudd will be set up for the current user.

Configuration
---------------------------------

At a bare minimum, bitcoincloudd requires that the rpcpassword setting be set
when running as a daemon.  If the configuration file does not exist or this
setting is not set, bitcoincloudd will shutdown promptly after startup.

This password does not have to be remembered or typed as it is mostly used
as a fixed token that bitcoincloudd and client programs read from the configuration
file, however it is recommended that a strong and secure password be used
as this password is security critical to securing the wallet should the
wallet be enabled.

If bitcoincloudd is run with the "-server" flag (set by default), and no rpcpassword is set,
it will use a special cookie file for authentication. The cookie is generated with random
content when the daemon starts, and deleted when it exits. Read access to this file
controls who can access it through RPC.

By default the cookie is stored in the data directory, but it's location can be overridden
with the option '-rpccookiefile'.

This allows for running bitcoincloudd without having to do any manual configuration.

`conf`, `pid`, and `wallet` accept relative paths which are interpreted as
relative to the data directory. `wallet` *only* supports relative paths.

For an example configuration file that describes the configuration settings,
see `share/examples/bitcoincloud.conf`.

Paths
---------------------------------

### Linux

All three configurations assume several paths that might need to be adjusted.

Binary:              `/usr/bin/bitcoincloudd`  
Configuration file:  `/etc/bitcoincloud/bitcoincloud.conf`  
Data directory:      `/var/lib/bitcoincloudd`  
PID file:            `/var/run/bitcoincloudd/bitcoincloudd.pid` (OpenRC and Upstart) or `/var/lib/bitcoincloudd/bitcoincloudd.pid` (systemd)  
Lock file:           `/var/lock/subsys/bitcoincloudd` (CentOS)  

The configuration file, PID directory (if applicable) and data directory
should all be owned by the bitcoincloud user and group.  It is advised for security
reasons to make the configuration file and data directory only readable by the
bitcoincloud user and group.  Access to bitcoincloud-cli and other bitcoincloudd rpc clients
can then be controlled by group membership.

### macOS

Binary:              `/usr/local/bin/bitcoincloudd`  
Configuration file:  `~/Library/Application Support/BitcoinCloud/bitcoincloud.conf`  
Data directory:      `~/Library/Application Support/BitcoinCloud`  
Lock file:           `~/Library/Application Support/BitcoinCloud/.lock`  

Installing Service Configuration
-----------------------------------

### systemd

Installing this .service file consists of just copying it to
/usr/lib/systemd/system directory, followed by the command
`systemctl daemon-reload` in order to update running systemd configuration.

To test, run `systemctl start bitcoincloudd` and to enable for system startup run
`systemctl enable bitcoincloudd`

NOTE: When installing for systemd in Debian/Ubuntu the .service file needs to be copied to the /lib/systemd/system directory instead.

### OpenRC

Rename bitcoincloudd.openrc to bitcoincloudd and drop it in /etc/init.d.  Double
check ownership and permissions and make it executable.  Test it with
`/etc/init.d/bitcoincloudd start` and configure it to run on startup with
`rc-update add bitcoincloudd`

### Upstart (for Debian/Ubuntu based distributions)

Upstart is the default init system for Debian/Ubuntu versions older than 15.04. If you are using version 15.04 or newer and haven't manually configured upstart you should follow the systemd instructions instead.

Drop bitcoincloudd.conf in /etc/init.  Test by running `service bitcoincloudd start`
it will automatically start on reboot.

NOTE: This script is incompatible with CentOS 5 and Amazon Linux 2014 as they
use old versions of Upstart and do not supply the start-stop-daemon utility.

### CentOS

Copy bitcoincloudd.init to /etc/init.d/bitcoincloudd. Test by running `service bitcoincloudd start`.

Using this script, you can adjust the path and flags to the bitcoincloudd program by
setting the BITCOINCLOUDD and FLAGS environment variables in the file
/etc/sysconfig/bitcoincloudd. You can also use the DAEMONOPTS environment variable here.

### macOS

Copy org.bitcoincloud.bitcoincloudd.plist into ~/Library/LaunchAgents. Load the launch agent by
running `launchctl load ~/Library/LaunchAgents/org.bitcoincloud.bitcoincloudd.plist`.

This Launch Agent will cause bitcoincloudd to start whenever the user logs in.

NOTE: This approach is intended for those wanting to run bitcoincloudd as the current user.
You will need to modify org.bitcoincloud.bitcoincloudd.plist if you intend to use it as a
Launch Daemon with a dedicated bitcoincloud user.

Auto-respawn
-----------------------------------

Auto respawning is currently only configured for Upstart and systemd.
Reasonable defaults have been chosen but YMMV.
