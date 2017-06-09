{
  "targets": [
    {
      "target_name": "addon",
      "sources": [ "srcs/addon.cc", "srcs/semaphore.cc" ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ]
    }
  ]
}
