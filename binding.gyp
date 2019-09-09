{
  "targets": [
    {
      "target_name": "addon",
      "sources": [ "srcs/semaphore.cc" ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ],
      "conditions": [
        ["<!(node -e \"console.log(process.versions.node.split(\'.\')[0])\") >= 11", {
          "sources": [ "srcs/addon_11+.cc" ]
        }],
        ["<!(node -e \"console.log(process.versions.node.split(\'.\')[0])\") < 11", {
          "sources": [ "srcs/addon.cc" ]
        }]
      ]
    }
  ]
}
