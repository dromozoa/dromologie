function handle(r)
  r.content_type = "application/json"
  r:puts "{}\n"
  return apache2.OK
end
