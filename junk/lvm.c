// gcc -o lvm{,.c} -Wall -g $(pkg-config --cflags --libs lvm2app devmapper) -lm

#include <stdio.h>
#include <lvm2app.h>
#include <math.h>

/**
 * error_message
 */
void
error_message (lvm_t l)
{
	printf ("err = %d\n", lvm_errno (l));
	printf ("msg = %s\n", lvm_errmsg (l));
}

/**
 * print_size
 */
char *
print_size (uint64_t size)
{
	static char buffer[64];
	double power = log2 ((double) size);
	const char *suffix = NULL;
	double divide = 1;

	if (power < 10) {
		suffix = "B";
		divide = 1;
	} else if (power < 20) {
		suffix = "KiB";
		divide = 1024;
	} else if (power < 30) {
		suffix = "MiB";
		divide = 1048576;
	} else if (power < 40) {
		suffix = "GiB";
		divide = 1073741824;
	} else if (power < 50) {
		suffix = "TiB";
		divide = 1099511627776;
	} else if (power < 60) {
		suffix = "PiB";
		divide = 1125899906842624;
	}
	sprintf (buffer, "%0.2f %s", (double)size/divide, suffix);
	return buffer;
}


/**
 * main
 */
int
main (int argc, char *argv[])
{
	lvm_t l;
	vg_t vg;
	struct dm_list *vgnames;
	struct dm_list *lvnames;
	struct dm_list *lvsegs;
	struct dm_list *pvnames;
	struct dm_list *pvsegs;
	struct lvm_str_list *strl;
	struct lvm_lv_list *lname;
	struct lvm_lvseg_list *seg;
	struct lvm_pv_list *pv;
	struct lvm_pvseg_list *pvseg;
	const char *vgname;
	const char *uuid;
	const char *name;
	uint64_t size_total;
	uint64_t size_free;
	uint64_t pv_count;
	lvm_property_value_t v;
	const char *prop1 = "seg_start"; // or seg_start_pe
	const char *prop2 = "seg_size";
	const char *prop3 = "seg_pe_ranges";
	const char *prop4 = "devices";

	l = lvm_init (NULL);
	//printf ("l = %p\n", l);

	if (lvm_scan (l) < 0) {
		error_message (l);
		goto quit;
	}

	vgnames = lvm_list_vg_names(l);
	dm_list_iterate_items(strl, vgnames) {
		vgname = strl->str;
		vg = lvm_vg_open(l, vgname, "r", 0);
		name = lvm_vg_get_name (vg);
		if (name[0] == 's') {
			uuid = lvm_vg_get_uuid (vg);
			size_total = lvm_vg_get_size (vg);
			size_free = lvm_vg_get_free_size (vg);
			pv_count = lvm_vg_get_pv_count (vg);
			printf ("name          %s\n", name);
			printf ("uuid          %s\n", uuid);
			printf ("total size    %s\n", print_size (size_total));
			printf ("free size     %s\n", print_size (size_free));
			printf ("phys vols     %lu\n", pv_count);

			lvnames = lvm_vg_list_lvs(vg);
			printf ("logical vols  %d\n", dm_list_size (lvnames));

			dm_list_iterate_items(lname, lvnames) {
				printf ("\t%s\n", lvm_lv_get_name (lname->lv));
				lvsegs = lvm_lv_list_lvsegs (lname->lv);
				dm_list_iterate_items(seg, lvsegs) {
					//printf ("\t\tseg: %p\n", seg->lvseg);

					v = lvm_lvseg_get_property(seg->lvseg, prop1);
					if (lvm_errno(l) || !v.is_valid) {
						printf("Invalid property name or unable to query '%s'.\n", prop1);
						continue;
					}
					printf("\t\t%lu", v.value.integer);

					v = lvm_lvseg_get_property(seg->lvseg, prop2);
					if (lvm_errno(l) || !v.is_valid) {
						printf("Invalid property name or unable to query '%s'.\n", prop2);
						continue;
					}
					printf(" (%s)", print_size (v.value.integer));

					v = lvm_lvseg_get_property(seg->lvseg, prop3);
					if (lvm_errno(l) || !v.is_valid) {
						printf("Invalid property name or unable to query '%s'.\n", prop3);
						continue;
					}
					printf(" (%s)", v.value.string);

					v = lvm_lvseg_get_property(seg->lvseg, prop4);
					if (lvm_errno(l) || !v.is_valid) {
						printf("Invalid property name or unable to query '%s'.\n", prop4);
						continue;
					}
					printf(" (%s)\n", v.value.string);
				}
			}
			pvnames = lvm_vg_list_pvs (vg);
			dm_list_iterate_items(pv, pvnames) {
				printf ("pv = %s: %s ", lvm_pv_get_name (pv->pv), print_size (lvm_pv_get_size (pv->pv)));
				printf ("(%s free)\n", print_size (lvm_pv_get_free (pv->pv)));
				pvsegs = lvm_pv_list_pvsegs (pv->pv);
				dm_list_iterate_items(pvseg, pvsegs) {
					//printf ("\t%p\n", pvseg->pvseg);
					v = lvm_pvseg_get_property(pvseg->pvseg, "pvseg_start");
					printf("\tpv start = %lu", v.value.integer);
					v = lvm_pvseg_get_property(pvseg->pvseg, "pvseg_size");
					printf(", size = %lu\n", v.value.integer);
				}
			}
		}
		lvm_vg_close(vg);
	}
quit:
	lvm_quit (l);
	return 0;
}

