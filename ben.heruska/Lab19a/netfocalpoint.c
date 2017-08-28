/* FocalPoint LKI */
/* Example: Register Net Device */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/socket.h>
#include <linux/in.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/sockios.h>
#include <linux/net.h>
#include <linux/inet.h>
#include <linux/if_arp.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <net/sock.h>
#include <linux/mm.h>
#include <linux/proc_fs.h>

#define DRIVER_AUTHOR "FocalPoint"
#define DRIVER_DESC   "Example: Add Net Device"

MODULE_LICENSE("GPL");           // Get rid of taint message by declaring code as GPL.

/*  Or with defines, like this: */
MODULE_AUTHOR(DRIVER_AUTHOR);    // Who wrote this module?
MODULE_DESCRIPTION(DRIVER_DESC); // What does this module do?

/* allow only 1 device instance */
static int focalpoint_ndevs = 1;

/* not really using this but need to declare
 * it to register net device.
 */
struct focalpoint_sock
{
	struct sock		sock;
};
/* not really using this but need to declare
 * it to register net device.
 */
static struct proto focalpoint_proto = {
	.name	  = "netfocalpoint",
	.owner	  = THIS_MODULE,
	.obj_size = sizeof(struct focalpoint_sock),
};

static struct net_device **dev_focalpoint;

static int focalpoint_xmit(struct sk_buff *skb, struct net_device *dev)
{
	/* just free the skb, we don't care */
	dev_kfree_skb(skb);
	return 0;
}

static const struct net_device_ops focalpoint_net_device_ops =
{
	.ndo_start_xmit = focalpoint_xmit,
};

static void focalpoint_setup(struct net_device *dev)
{
	/* focalpoint is a TUNNEL device! */
	dev->type = ARPHRD_TUNNEL;
	dev->netdev_ops = &focalpoint_net_device_ops;
	//dev->hard_start_xmit = focalpoint_xmit; deprecated in kernel 2.6.31
}

static int init(void)
{
	int result;
	struct net_device *dev;
	char name[16];

	result = proto_register(&focalpoint_proto, 0);
	if (result != 0) goto out;
	/* kzalloc = kmalloc + memset 0x0 */
	dev_focalpoint = kzalloc(focalpoint_ndevs * sizeof(struct net_device *), GFP_KERNEL);
	if (dev_focalpoint == NULL)
	{
		printk(KERN_ERR "netfocalpoint: focalpoint_proto_init - fail\n");
		result = -ENOMEM;
		goto out;
	}
	/* only 1 device so we just call it focalpoint0 */
	sprintf(name, "focalpoint%d", 0);
	dev = alloc_netdev(sizeof(struct net_device_stats), name, focalpoint_setup);
	if (!dev) {
		printk(KERN_ERR "netfocalpoint: focalpoint_proto_init - mem fail\n");
		result = -ENOMEM;
		goto fail;
	}
	/* register our new device */
	result = register_netdev(dev);
	if (result) {
		printk(KERN_ERR "netfocalpoint: netdevice registration failed\n");
		free_netdev(dev);
		goto fail;
	}
	dev_focalpoint[0] = dev;
out:
	return result;
fail:
	unregister_netdev(dev_focalpoint[0]);
	free_netdev(dev_focalpoint[0]);
	kfree(dev_focalpoint);

	return 0;
}

static void exit(void)
{
	struct net_device *dev = dev_focalpoint[0];

	/* undo everything we did to install net device */
	//proc_net_remove("netfocalpoint");
	unregister_netdev(dev);
	free_netdev(dev);
	kfree(dev_focalpoint);
	proto_unregister(&focalpoint_proto);

}

module_init(init);
module_exit(exit);
