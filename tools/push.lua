#! /usr/bin/env lua

---------------------------------------------------------------------------

local root = os.getenv("HOME").."/Workspace/dromologie"

---------------------------------------------------------------------------

local base64_encoder = {
  [62] = "-";
  [63] = "_";
}

local n = ("A"):byte()
for i = 0, 25 do
  base64_encoder[i] = string.char(n + i)
end

local n = ("a"):byte() - 26
for i = 26, 51 do
  base64_encoder[i] = string.char(n + i)
end

local n = ("0"):byte() - 52
for i = 52, 61 do
  base64_encoder[i] = string.char(n + i)
end

local base64_decoder = {}

for i = 0, 63 do
  base64_decoder[base64_encoder[i]:byte()] = i
end

local function encode_base64_url(source)
  local result = {}
  for i = 1, #source, 3 do
    local a, b, c = source:byte(i, i + 2)
    local n = #result
    if c then
      local v = a << 16 | b << 8 | c
      result[#result + 1] = base64_encoder[v >> 18]..base64_encoder[v >> 12 & 0x3F]..base64_encoder[v >> 6 & 0x3F]..base64_encoder[v & 0x3F]
    elseif b then
      local v = a << 10 | b << 2
      result[#result + 1] = base64_encoder[v >> 12]..base64_encoder[v >> 6 & 0x3F]..base64_encoder[v & 0x3F]
    else
      local v = a << 4
      result[#result + 1] = base64_encoder[v >> 6]..base64_encoder[v & 0x3F]
    end
  end
  return table.concat(result)
end

local function decode_base64_url(source)
  local result = {}
  for i = 1, #source, 4 do
    local a, b, c, d = source:byte(i, i + 3)
    if d then
      local v = base64_decoder[a] << 18 | base64_decoder[b] << 12 | base64_decoder[c] << 6 | base64_decoder[d]
      result[#result + 1] = string.char(v >> 16, v >> 8 & 0xFF, v & 0xFF)
    elseif c then
      local v = base64_decoder[a] << 10 | base64_decoder[b] << 4 | base64_decoder[c] >> 2
      result[#result + 1] = string.char(v >> 8, v & 0xFF)
    else
      local v = base64_decoder[a] << 2 | base64_decoder[b] >> 4
      result[#result + 1] = string.char(v)
    end
  end
  return table.concat(result)
end

---------------------------------------------------------------------------

local function encode_hex(source)
  local result = {}
  for i = 1, #source do
    result[i] = ("%02X"):format(source:byte(i))
  end
  return table.concat(result)
end

local function random(n)
  local handle <close> = assert(io.open("/dev/urandom"))
  return handle:read(n)
end

---------------------------------------------------------------------------

local auto_remover_metatable = {}

function auto_remover_metatable:__close()
  os.remove(self.filename)
end

local function auto_remove(filename)
  return setmetatable({ filename = filename }, auto_remover_metatable)
end

---------------------------------------------------------------------------

local tmp <close> = auto_remove(os.tmpname())

local function quote(source)
  return [[']]..source:gsub([[']], [['\'']])..[[']]
end

local function generate_private_key()
  local command = "openssl ecparam -genkey -name prime256v1 -noout"
  local handle <close> = assert(io.popen(command))
  return handle:read "*a"
end

local function read_private_key(key_path)
  local handle <close> = assert(io.open(key_path))
  return handle:read "*a"
end

local function get_public_key(private_key)
  do
    local command = ("openssl ec -pubout >%s 2>/dev/null"):format(quote(tmp.filename))
    local handle <close> = assert(io.popen(command, "w"))
    handle:write(private_key)
  end
  local handle <close> = assert(io.open(tmp.filename))
  return handle:read "*a"
end

local function export_public_key(public_key)
  do
    local command = ("openssl pkey -pubin -outform DER >%s"):format(quote(tmp.filename))
    local handle <close> = assert(io.popen(command, "w"))
    handle:write(public_key)
  end
  local handle <close> = assert(io.open(tmp.filename))
  local data = handle:read "*a"
  return encode_base64_url(data:sub(-65))
end

local function import_public_key(public_key)
  local data
  do
    local command = "openssl ecparam -genkey -name prime256v1 -noout | openssl ec -pubout -outform DER 2>/dev/null"
    local handle <close> = assert(io.popen(command))
    data = handle:read "*a"
  end
  do
    local command = ("openssl pkey -pubin -inform DER >%s"):format(quote(tmp.filename))
    local handle <close> = assert(io.popen(command, "w"))
    handle:write(data:sub(1, -66), decode_base64_url(public_key))
  end
  local handle <close> = assert(io.open(tmp.filename))
  return handle:read "*a"
end

local function ecdh(private_key, public_key)
  do
    local handle <close> = assert(io.open(tmp.filename, "w"))
    handle:write(public_key)
  end
  local out <close> = auto_remove(os.tmpname())
  local command = ("openssl pkeyutl -derive -inkey /dev/stdin -peerkey %s >%s"):format(quote(tmp.filename), quote(out.filename))
  do
    local handle <close> = assert(io.popen(command, "w"))
    handle:write(private_key)
  end
  local handle <close> = assert(io.open(out.filename))
  return handle:read "*a"
end

local function hmac_sha_256(key, message)
  do
    local command = ("openssl dgst -sha256 -mac HMAC -macopt hexkey:%s -binary >%s"):format(encode_hex(key), quote(tmp.filename))
    local handle <close> = assert(io.popen(command, "w"))
    handle:write(message)
  end
  local handle <close> = assert(io.open(tmp.filename))
  return handle:read "*a"
end

local function aes_128_gcm(key, iv, message)
  do
    local command = ("%s/bin/aes128gcm %s %s >%s"):format(quote(root), encode_base64_url(key), encode_base64_url(iv), quote(tmp.filename))
    local handle <close> = assert(io.popen(command, "w"))
    handle:write(message)
  end
  local handle <close> = assert(io.open(tmp.filename))
  return handle:read "*a"
end

local function ecdsa(key_path, message)
  do
    local handle <close> = assert(io.open(tmp.filename, "w"))
    handle:write(message)
  end
  local command = ("cat %s | %s/bin/ecdsa `%s/bin/sha256 <%s | %s/bin/encode_base64url`"):format(quote(key_path), quote(root), quote(root), quote(tmp.filename), quote(root))
  local handle <close> = assert(io.popen(command))
  return handle:read "*a"
end

local key_path, endpoint, p256dh, secret, sub = ...
local message = io.read "*a"

local aud = assert(endpoint:match "^(https://[^/]+)")

local jwt_header = encode_base64_url [[{"typ":"JWT","alg":"ES256"}]]
local jwt_body = encode_base64_url(([[{"aud":"%s","exp":%d,"sub":"%s"}]]):format(aud, os.time() + 12 * 3600, sub))
local jwt_signature = encode_base64_url(ecdsa(key_path, jwt_header.."."..jwt_body))
local public_key = export_public_key(get_public_key(read_private_key(key_path)))
local authorization = ("vapid t=%s.%s.%s,k=%s"):format(jwt_header, jwt_body, jwt_signature, public_key)

-- pem
local as_private = generate_private_key()
local as_public = get_public_key(as_private)
local ua_public = import_public_key(p256dh)

-- bin
local as_public_bin = decode_base64_url(export_public_key(as_public))
local ua_public_bin = decode_base64_url(export_public_key(ua_public))

-- binary
local ecdh_secret = ecdh(as_private, ua_public)
local auth_secret = decode_base64_url(secret)
local salt = random(16)

local prk_key = hmac_sha_256(auth_secret, ecdh_secret)
local key_info = "WebPush: info\0"..ua_public_bin..as_public_bin
local ikm = hmac_sha_256(prk_key, key_info.."\1")

local prk = hmac_sha_256(salt, ikm)
local cek_info = "Content-Encoding: aes128gcm\0"
local cek = hmac_sha_256(prk, cek_info.."\1"):sub(1, 16)
local nonce_info = "Content-Encoding: nonce\0"
local nonce = hmac_sha_256(prk, nonce_info.."\1"):sub(1, 12)

-- 最後のレコードは短くなっても構わない。
local record_size = 4096
local rs = (">I4"):pack(record_size)
local header = salt..rs..string.char(#as_public_bin)..as_public_bin
local cipher_text = aes_128_gcm(cek, nonce, message.."\2")

io.stderr:write(([[
Authorization: %s
]]):format(authorization))
io.write(header, cipher_text)

--[====[

print(encode_base64_url(cipher_text))

]====]
