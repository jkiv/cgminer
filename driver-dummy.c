#include <unistd.h>

#include "logging.h"
#include "miner.h"

struct dummy_info
{
  struct cgpu_info *cgpu;

  // Statistics
  uint64_t jobs_completed_good;
  uint64_t jobs_completed_bad;
  uint64_t jobs_flushed;
};

void dummy_detect(bool __maybe_unused hotplug)
{
  struct cgpu_info *cgpu = NULL;
  struct dummy_info *dmy = NULL;

  applog(LOG_INFO, "dummy_detect()");

  cgpu = calloc(1, sizeof(struct cgpu_info));

  if (!cgpu) {
    applog(LOG_ERR, "dummy_detect: failed to allocate struct cgpu_info.");
    return;
  }

  dmy = calloc(1, sizeof(struct dummy_info));

  if (!dmy) {
    applog(LOG_ERR, "dummy_detect: failed to allocate struct dummy_info.");
    free(cgpu);
    return;
  }

  // Detect devices here

  // .. dummy has no real devices!

  // Populate cgpu structure
  cgpu->drv = &dummy_drv;
  cgpu->device_data = dmy;
  cgpu->name = "Dummy";
  cgpu->threads = 1;

  // Populate dummy info structure
  dmy->cgpu = cgpu;

  dmy->jobs_completed_good = 0;
  dmy->jobs_completed_bad = 0;
  dmy->jobs_flushed = 0;

  // TODO both structures reference each other,
  //      how to protect against use-after-free?

  // Call back to cgminer API
  if(!add_cgpu(cgpu)) {
    applog(LOG_ERR, "dummy_detect: call to add_cgpu failed.");
  }
}

/* Tell the driver of a block change */
static void dummy_flush_work(struct cgpu_info *cgpu)
{
  applog(LOG_INFO, "dummy_flush_work()");
  // TODO scanadjust?

  if (cgpu == NULL) {
    applog(LOG_ERR, "dummy_flush_work: cgpu is NULL.");
    return;
  }

  struct dummy_info* dmy = cgpu->device_data;

  if (dmy == NULL) {
    applog(LOG_ERR, "dummy_flush_work: dmy is NULL.");
    return;
  }

  dmy->jobs_flushed++;
}


/* Two variants depending on whether the device divides work up into
 * small pieces or works with whole work items and may or may not have
 * a queue of its own. */
static int64_t dummy_scanhash(struct thr_info *thr, struct work *work, int64_t what_is_this)
{
  // TODO is werk.midstate what I'm interested in?
  // TODO is werk.data what I'm interested in?
  // TODO compute diff offset from werk.device_diff?
  // TODO compute diff offset from werk.target?
  // TODO what is final int64_t?

  applog(LOG_INFO, "dummy_scanhash():");

  unsigned char *p;

  applog(LOG_INFO, "  .data");
  for (p = work->data; p < work->data + 128; p += 8) {
    applog(LOG_INFO, "    %02X%02X%02X%02X %02X%02X%02X%02X", p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
  }

  applog(LOG_INFO, "  .midstate");
  for (p = work->midstate; p < work->midstate + 32; p += 8) {
    applog(LOG_INFO, "    %02X%02X%02X%02X %02X%02X%02X%02X", p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
  }

  applog(LOG_INFO, "  .target");
  for (p = work->target; p < work->target + 32; p += 8) {
    applog(LOG_INFO, "    %02X%02X%02X%02X %02X%02X%02X%02X", p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
  }

  applog(LOG_INFO, "  .device_diff");
  applog(LOG_INFO, "    %e", work->device_diff);
  applog(LOG_INFO, "  .work_difficulty");
  applog(LOG_INFO, "    %e", work->work_difficulty);
  applog(LOG_INFO, "  .share_diff");
  applog(LOG_INFO, "    %lld", work->share_diff);

  sleep(60); // Simulate work

  return 0; // TODO what does return value represent?
}

/* Two variants depending on whether the device divides work up into
 * small pieces or works with whole work items and may or may not have
 * a queue of its own. */
static int64_t dummy_scanwork(struct thr_info *thr)
{
  applog(LOG_INFO, "dummy_scanwork()");

  struct work *work = get_work(thr, thr->id);

  if (!work) {
    applog(LOG_ERR, "dummy_scanwork: call to get_work() failed.");
    return; // TODO what does return value represent?
  }

  unsigned char *p;

  applog(LOG_INFO, "  .data");
  for (p = work->data; p < work->data + 128; p += 8) {
    applog(LOG_INFO, "    %02X%02X%02X%02X %02X%02X%02X%02X", p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
  }

  applog(LOG_INFO, "  .midstate");
  for (p = work->midstate; p < work->midstate + 32; p += 8) {
    applog(LOG_INFO, "    %02X%02X%02X%02X %02X%02X%02X%02X", p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
  }

  applog(LOG_INFO, "  .target");
  for (p = work->target; p < work->target + 32; p += 8) {
    applog(LOG_INFO, "    %02X%02X%02X%02X %02X%02X%02X%02X", p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
  }

  applog(LOG_INFO, "  .device_diff");
  applog(LOG_INFO, "    %e", work->device_diff);
  applog(LOG_INFO, "  .work_difficulty");
  applog(LOG_INFO, "    %e", work->work_difficulty);
  applog(LOG_INFO, "  .share_diff");
  applog(LOG_INFO, "    %lld", work->share_diff);

  sleep(60); // Simulate work

  /* return value is number of nonces that have been checked since
   * previous call
   */
  return 0;
}

struct device_drv dummy_drv = {
	.drv_id = DRIVER_dummy,
	.dname = "Dummy driver",
	.name = "Dummy",
	.drv_detect = dummy_detect,
	.hash_work = hash_queued_work,
	.flush_work = dummy_flush_work,
	.scanwork = dummy_scanwork,
	.scanhash = dummy_scanhash,
	//.max_diff = 4.0,
	//.min_diff = 1.0
};
