{
  stdenv
, source ? ./.
, ImAnimateSource
, MumbleApiSource
, NexusTranslations ? null
, cmake
, pkg-config
, nlohmann_json
, openssl
, httplib
, stb
, mcfgthreads ? windows.mcfgthreads
, pthreads ? windows.pthreads
, windows
}: stdenv.mkDerivation {
  pname = "nexus";
  version = "0.0.0";
  src = source;

  nativeBuildInputs = [ cmake pkg-config ];
  buildInputs = [
    nlohmann_json openssl httplib stb
    #pthreads
    mcfgthreads
  ];

  inherit ImAnimateSource MumbleApiSource NexusTranslations;
  postPatch = ''
    mkdir -p src/thirdparty/ImAnimate
    cp --no-preserve=mode $ImAnimateSource/ImAnimate.{cpp,h} \
      src/thirdparty/ImAnimate
    mkdir -p src/Services/Mumble/Definitions/
    cp --no-preserve=mode $MumbleApiSource/Mumble.h \
      src/Services/Mumble/Definitions/
  '';
}
