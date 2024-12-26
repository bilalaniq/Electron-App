{
  "targets": [
    {
      "target_name": "addon",
      "sources": [ "CyberMatch.cc" ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ],
      "libraries": [],
      "defines": [],
      "output_dir": "./addon"
    }
  ]
}

# {
#   "targets": [
#     {
#       "target_name": "addon",
#       "sources": ["CyberMatch.cc"]
#     }
#   ]
# }
