#include "legacy/v4l2.h"

static query_node* add_query_node(
    query_node* query,
    char* key,
    void* query_value,
    long unsigned int query_value_len) {
  size_t len = strlen(key);
  query_node* new_node = malloc(sizeof(query_node));
  new_node->key = malloc(len + 1);
  memcpy(new_node->key, key, len);
  new_node->key[len] = 0; // null terminator for strcmp

  new_node->value = malloc(query_value_len);
  memcpy(new_node->value, query_value, sizeof query_value_len);
  new_node->next = query;
  return new_node;
}

static void release_node(query_node* query) {
  query_node* ptr = query;
  query_node* curp;
  while (ptr) {
    curp = ptr;
    ptr = ptr->next;
    free(curp->key);
    free(curp->value);
    free(curp);
  }
}

static void* get_query_node(query_node* query, char const* key) {
  query_node* qptr = query;
  while (qptr) {
    if (!strcmp(qptr->key, key)) {
      return qptr->value;
    }
    qptr = qptr->next;
  }
  return 0;
}

static int xioctl(int fd, unsigned long request, void* arg) {
  int r;
  do {
    r = ioctl(fd, request, arg);
  } while (r == -1 && errno == EINTR);
  return r;
}

int v4l2_query_menu(v4l2_device* vdev, struct v4l2_queryctrl* queryctrl) {
  struct v4l2_querymenu querymenu;

  querymenu.id = queryctrl->id;
  for (querymenu.index = (unsigned)queryctrl->minimum;
       querymenu.index <= (unsigned)queryctrl->maximum;
       querymenu.index++) {
    if (!ioctl(vdev->fd, VIDIOC_QUERYMENU, &querymenu)) {
      vdev->menu_map = add_query_node(vdev->menu_map,
                                      (char*)querymenu.name, &querymenu, sizeof querymenu);
    } else {
      fprintf(stderr, "Could not query menu %d\n", querymenu.index);
    }
  }
  return 0;
}

int v4l2_query_ctrl(v4l2_device* vdev, unsigned int addr_begin, unsigned int addr_end) {
  struct v4l2_queryctrl queryctrl;

  for (queryctrl.id = addr_begin; queryctrl.id < addr_end; queryctrl.id++) {
    if (ioctl(vdev->fd, VIDIOC_QUERYCTRL, &queryctrl) == -1) {
      if (errno == EINVAL) {
        continue;
      } else {
        fprintf(stderr, "Could not query control %d\n", queryctrl.id);
      }
    }
    printf("queryctrl: \"%s\" 0x%x %d %d %d\n",
           queryctrl.name, queryctrl.id, queryctrl.minimum,
           queryctrl.maximum, queryctrl.default_value);
    fflush(stdout);

    switch (queryctrl.type) {
    case V4L2_CTRL_TYPE_MENU:
      v4l2_query_menu(vdev, &queryctrl);
    case V4L2_CTRL_TYPE_INTEGER:
    case V4L2_CTRL_TYPE_BOOLEAN:
    case V4L2_CTRL_TYPE_BUTTON:
      vdev->ctrl_map = add_query_node(vdev->ctrl_map,
                                      (char*)queryctrl.name, &queryctrl, sizeof queryctrl);
      break;
    default:
      break;
    }
  }

  return 0;
}

int v4l2_error(char const* error_msg) {
  int x = errno;
  fprintf(stderr, "Err: %d\n", x);
  fprintf(stderr, "V4L2 error: %s\n", error_msg);
  return -2;
}

int v4l2_uninit_mmap(v4l2_device* vdev) {
  for (unsigned int i = 0; i < NBUFFERS; ++i) {
    if (munmap(vdev->buffer[i], vdev->buf_len[i]) == -1) {
      return v4l2_error("munmap");
    }
  }
  free(vdev->buffer);
  free(vdev->buf_len);
  return 0;
}

int v4l2_init_mmap(v4l2_device* vdev) {
  struct v4l2_requestbuffers req;
  req.count = NBUFFERS;
  req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory = V4L2_MEMORY_MMAP;
  if (xioctl(vdev->fd, VIDIOC_REQBUFS, &req)) {
    return v4l2_error("VIDIOC_REQBUFS");
  }
  if (req.count < 2) {
    return v4l2_error("Insufficient buffer memory\n");
  }

  vdev->buffer = malloc(req.count * sizeof(void*));
  vdev->buf_len = malloc(req.count * sizeof(int));

  struct v4l2_buffer buf;
  for (unsigned int i = 0; i < req.count; ++i) {
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = i;
    if (xioctl(vdev->fd, VIDIOC_QUERYBUF, &buf) == -1) {
      return v4l2_error("VIDIOC_QUERYBUF");
    }
    vdev->buf_len[i] = buf.length;
    vdev->buffer[i] = mmap(
        NULL, // start anywhere
        buf.length,
        PROT_READ | PROT_WRITE, // required
        MAP_SHARED,             // recommended
        vdev->fd,
        buf.m.offset);
    if (vdev->buffer[i] == MAP_FAILED) {
      return v4l2_error("mmap");
    }
    printf("buffer length %d\n", vdev->buf_len[i]);
  }

  return 0;
}

int v4l2_open(char const* device) {
  int video_fd = open(device, O_RDWR); // open video device with system call
  if (video_fd == -1) {
    return v4l2_error("Could not open video device");
  } else {
    return video_fd;
  }
}

int v4l2_close_query(v4l2_device* vdev) {
  release_node(vdev->ctrl_map);
  release_node(vdev->menu_map);
  return 0;
}

int v4l2_close(v4l2_device* vdev) {
  v4l2_uninit_mmap(vdev);
  // TODO: free control (this comment is from the original legacy C file--still relevant??)
  v4l2_close_query(vdev);
  if (close(vdev->fd) == -1) {
    return v4l2_error("Closing video device");
  }
  vdev->fd = -1;
  return 0;
}

int v4l2_init(v4l2_device* vdev) {
  struct v4l2_capability video_cap;
  // check if capture and streaming device
  if (xioctl(vdev->fd, VIDIOC_QUERYCAP, &video_cap) == -1) {
    return v4l2_error("VIDIOC_QUERYCAP");
  }
  if (!(video_cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
    return v4l2_error("No video capture device");
  }
  if (!(video_cap.capabilities & V4L2_CAP_STREAMING)) {
    return v4l2_error("No capture streaming");
  }

  // Get current format
  struct v4l2_format video_fmt;
  video_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  video_fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
  if (xioctl(vdev->fd, VIDIOC_G_FMT, &video_fmt) == -1) {
    return v4l2_error("VIDIOC_G_FMT: Failed to get format");
  }

  // Set video format, such as width, height, pixelformat
  video_fmt.fmt.pix.width = vdev->width;
  video_fmt.fmt.pix.height = vdev->height;
  if (!strcmp(vdev->pixelformat, "yuyv")) {
    video_fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
  } else if (!strcmp(vdev->pixelformat, "mjpeg")) {
    video_fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
  } else if (!strcmp(vdev->pixelformat, "uyvy")) {
    video_fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_UYVY; // iSight
  } else {
    video_fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fprintf(stderr, "unknown pixelformat, use YUYV in default");
  }
  video_fmt.fmt.pix.field = V4L2_FIELD_NONE;
  if (xioctl(vdev->fd, VIDIOC_S_FMT, &video_fmt) == -1) {
    return v4l2_error("VIDIOC_S_FMT: Failed to set format");
  }

  printf("Current format\n");
  printf("+------------+\n");
  printf("Width: %u\n", video_fmt.fmt.pix.width);
  printf("Height: %u\n", video_fmt.fmt.pix.height);
  printf("Pixel format: %u\n", video_fmt.fmt.pix.pixelformat);
  printf("Pixel field: %u\n", video_fmt.fmt.pix.field);

  printf("Start querying\n");
  fflush(stdout);

  v4l2_query_ctrl(vdev, V4L2_CID_BASE, V4L2_CID_LASTP1 + 100);
  v4l2_query_ctrl(vdev, V4L2_CID_CAMERA_CLASS_BASE, V4L2_CID_PRIVACY + 100);
  v4l2_query_ctrl(vdev, V4L2_CID_PRIVATE_BASE, V4L2_CID_PRIVATE_BASE + 100);

  // Set desired frame rate
  printf("Setting frame rate...\n");
  fflush(stdout);
  struct v4l2_streamparm streamparm;
  streamparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (xioctl(vdev->fd, VIDIOC_G_PARM, &streamparm) == -1) {
    return v4l2_error("failed to get stream parameters");
  }

  // Nao driver requires set default frame rate to (1/0) not (1/30)
  streamparm.parm.capture.timeperframe.numerator = 1;
  streamparm.parm.capture.timeperframe.denominator = 0;
  if (xioctl(vdev->fd, VIDIOC_S_PARM, &streamparm) == -1) {
    return v4l2_error("failed to set frame rate");
  }
  printf("frame rate: %d/%d\n",
         streamparm.parm.capture.timeperframe.numerator,
         streamparm.parm.capture.timeperframe.denominator);
  fflush(stdout);

  v4l2_init_mmap(vdev);

  return 0;
}

int v4l2_stream_on(v4l2_device* vdev) {
  printf("IN v4l2_stream_on %d \n", vdev->count);
  struct v4l2_buffer buf;
  for (unsigned int i = 0; i < NBUFFERS; i++) {
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = i;
    if (xioctl(vdev->fd, VIDIOC_QBUF, &buf) == -1) {
      return v4l2_error("VIDIOC_QBUF");
    }
  }

  enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  printf("fd %d stream on\n", vdev->fd);
  if (xioctl(vdev->fd, VIDIOC_STREAMON, &type) == -1) {
    return v4l2_error("VIDIOC_STREAMON");
  }
  return 0;
}

int v4l2_stream_off(v4l2_device* vdev) {
  enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (xioctl(vdev->fd, VIDIOC_STREAMOFF, &type) == -1) {
    return v4l2_error("VIDIOC_STREAMOFF");
  }
  return 0;
}

int v4l2_get_ctrl(v4l2_device* vdev, char const* name, int* value) {
  struct v4l2_queryctrl* ictrl = get_query_node(vdev->ctrl_map, name);
  if (!ictrl) {
    fprintf(stderr, "Unknown control '%s'\n", name);
    return -1;
  }

  struct v4l2_control ctrl;
  ctrl.id = ictrl->id;
  int ret = xioctl(vdev->fd, VIDIOC_G_CTRL, &ctrl);
  *value = ctrl.value;
  return ret;
}

int v4l2_set_ctrl(v4l2_device* vdev, char const* name, int value) {
  struct v4l2_queryctrl* ictrl = get_query_node(vdev->ctrl_map, name);
  if (!ictrl) {
    fprintf(stderr, "Unknown control '%s'\n", name);
    return -1;
  }

  fprintf(stderr, "Setting ctrl %s, id %d\n", name, ictrl->id - V4L2_CID_BASE);
  struct v4l2_control ctrl;
  ctrl.id = ictrl->id;
  ctrl.value = value;
  return xioctl(vdev->fd, VIDIOC_S_CTRL, &ctrl);
}

int v4l2_read_frame(v4l2_device* vdev) {
  struct v4l2_buffer buf;
  buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buf.memory = V4L2_MEMORY_MMAP;
  if (xioctl(vdev->fd, VIDIOC_DQBUF, &buf) == -1) {
    switch (errno) {
    case EAGAIN:
      // printf("no frame available\n");
      return -1;
    // case EIO: // Could ignore EIO
    default:
      return v4l2_error("VIDIOC_DQBUF");
    }
  }
  assert(buf.index < NBUFFERS);
  if (xioctl(vdev->fd, VIDIOC_QBUF, &buf) == -1) {
    fprintf(stderr, "QBUF Problem %d\n", errno);
    fprintf(stderr, "Buf Index: %d\n", buf.index);
    fprintf(stderr, "Buf Type: 0x%X\n", buf.type);
    return v4l2_error("VIDIOC_QBUF");
  }

  return buf.index;
}
