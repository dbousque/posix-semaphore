{
  "targets": [
    {
      "target_name": "addon",
      "sources": [ "addon.cc", "semaphore.cc" ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ]
    }
  ]
}
