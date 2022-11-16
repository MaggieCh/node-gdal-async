{
	"includes": [
		"./common.gypi",
		"./libgdal_formats.gypi"
	],
	"targets": [
		{
			"target_name": "libgdal",
			"type": "static_library",
			# It would be nice to glob these folders, but there are some exclusions :(
			"sources": [
				"./gdal/apps/ogr2ogr_lib.cpp",
				"./gdal/apps/gdalbuildvrt_lib.cpp",
        "./gdal/apps/gdal_translate_lib.cpp",
        "./gdal/apps/gdal_rasterize_lib.cpp",
        "./gdal/apps/gdalinfo_lib.cpp",
        "./gdal/apps/gdalwarp_lib.cpp",
				"./gdal/apps/commonutils.cpp",
				"./gdal/frmts/gdalallregister.cpp",
				"./gdal/frmts/derived/deriveddataset.cpp",
				"./gdal/frmts/derived/derivedlist.c",

				# "./gdal/ogr/test_geo_utils.cpp",
				# "./gdal/ogr/ogr_capi_test.c",
				"./gdal/ogr/swq_select.cpp",
				"./gdal/ogr/swq_parser.cpp",
				"./gdal/ogr/swq_op_registrar.cpp",
				"./gdal/ogr/swq_op_general.cpp",
				"./gdal/ogr/swq_expr_node.cpp",
				"./gdal/ogr/swq.cpp",
				"./gdal/ogr/ogrutils.cpp",
				"./gdal/ogr/ogrtriangulatedsurface.cpp",
				"./gdal/ogr/ogrtriangle.cpp",
				"./gdal/ogr/ogrsurface.cpp",
				"./gdal/ogr/ogrspatialreference.cpp",
				"./gdal/ogr/ogrpolyhedralsurface.cpp",
				"./gdal/ogr/ogrpolygon.cpp",
				"./gdal/ogr/ogrpoint.cpp",
				"./gdal/ogr/ogrpgeogeometry.cpp",
				"./gdal/ogr/ogrmultisurface.cpp",
				"./gdal/ogr/ogrmultipolygon.cpp",
				"./gdal/ogr/ogrmultipoint.cpp",
				"./gdal/ogr/ogrmultilinestring.cpp",
				"./gdal/ogr/ogrmulticurve.cpp",
				"./gdal/ogr/ogrlinestring.cpp",
				"./gdal/ogr/ogrlinearring.cpp",
				"./gdal/ogr/ogrgeomfielddefn.cpp",
				"./gdal/ogr/ogrgeometryfactory.cpp",
				"./gdal/ogr/ogrgeometrycollection.cpp",
				"./gdal/ogr/ogrgeometry.cpp",
				"./gdal/ogr/ogrfielddefn.cpp",
				"./gdal/ogr/ogrfeaturestyle.cpp",
				"./gdal/ogr/ogrfeaturequery.cpp",
				"./gdal/ogr/ogrfeaturedefn.cpp",
				"./gdal/ogr/ogrfeature.cpp",
				"./gdal/ogr/ogrcurvepolygon.cpp",
				"./gdal/ogr/ogrcurvecollection.cpp",
				"./gdal/ogr/ogrcurve.cpp",
				"./gdal/ogr/ogrct.cpp",
				"./gdal/ogr/ogrcompoundcurve.cpp",
				"./gdal/ogr/ogrcircularstring.cpp",
				"./gdal/ogr/ograssemblepolygon.cpp",
				"./gdal/ogr/ograpispy.cpp",
				"./gdal/ogr/ogr_xerces.cpp",
				"./gdal/ogr/ogr_srsnode.cpp",
				"./gdal/ogr/ogr_srs_xml.cpp",
				"./gdal/ogr/ogr_srs_usgs.cpp",
				"./gdal/ogr/ogr_srs_pci.cpp",
				"./gdal/ogr/ogr_srs_panorama.cpp",
				"./gdal/ogr/ogr_srs_ozi.cpp",
				"./gdal/ogr/ogr_srs_esri.cpp",
				"./gdal/ogr/ogr_srs_erm.cpp",
				"./gdal/ogr/ogr_srs_dict.cpp",
				"./gdal/ogr/ogr_proj_p.cpp",
				"./gdal/ogr/ogr_geocoding.cpp",
				"./gdal/ogr/ogr_geo_utils.cpp",
				"./gdal/ogr/ogr_fromepsg.cpp",
				"./gdal/ogr/ogr_expat.cpp",
				"./gdal/ogr/ogr_api.cpp",
				"./gdal/ogr/ogr_wkb.cpp",
				"./gdal/ogr/ogr2gmlgeometry.cpp",
				"./gdal/ogr/gml2ogrgeometry.cpp",
				# "./gdal/ogr/generate_encoding_table.c",
				"./gdal/ogr/ogrsf_frmts/generic/ogr_gensql.cpp",
				"./gdal/ogr/ogrsf_frmts/generic/ogremulatedtransaction.cpp",
				"./gdal/ogr/ogrsf_frmts/generic/ogr_miattrind.cpp",
				"./gdal/ogr/ogrsf_frmts/generic/ogrunionlayer.cpp",
				"./gdal/ogr/ogrsf_frmts/generic/ogrsfdriver.cpp",
				"./gdal/ogr/ogrsf_frmts/generic/ogrregisterall.cpp",
				"./gdal/ogr/ogrsf_frmts/generic/ogreditablelayer.cpp",
				"./gdal/ogr/ogrsf_frmts/generic/ogr_attrind.cpp",
				"./gdal/ogr/ogrsf_frmts/generic/ogrdatasource.cpp",
				"./gdal/ogr/ogrsf_frmts/generic/ogrmutexedlayer.cpp",
				"./gdal/ogr/ogrsf_frmts/generic/ogrwarpedlayer.cpp",
				"./gdal/ogr/ogrsf_frmts/generic/ogrlayerpool.cpp",
				"./gdal/ogr/ogrsf_frmts/generic/ograrrowarrayhelper.cpp",
				"./gdal/ogr/ogrsf_frmts/generic/ogrsfdriverregistrar.cpp",
				"./gdal/ogr/ogrsf_frmts/generic/ogrmutexeddatasource.cpp",
				"./gdal/ogr/ogrsf_frmts/generic/ogrlayer.cpp",
				"./gdal/ogr/ogrsf_frmts/generic/ogrlayerdecorator.cpp",

				'<!@(python ../glob-files.py "./gdal/alg/*.cpp")',
				'<!@(python ../glob-files.py "./gdal/alg/*.c")',
				'<!@(python ../glob-files.py "./gdal/gcore/*.cpp")',
				'<!@(python ../glob-files.py "./gdal/gcore/mdreader/*.cpp")',

				# "./gdal/port/vsipreload.cpp",
				# "./gdal/port/cpl_vsil_simple.cpp",
				"./gdal/port/cplstringlist.cpp",
				"./gdal/port/cplstring.cpp",
				"./gdal/port/cpl_float.cpp",
				"./gdal/port/cplkeywordparser.cpp",
				"./gdal/port/cplgetsymbol.cpp",
				"./gdal/port/cpl_xml_validate.cpp",
				"./gdal/port/cpl_worker_thread_pool.cpp",
				"./gdal/port/cpl_vsisimple.cpp",
				"./gdal/port/cpl_vsil_win32.cpp",
				"./gdal/port/cpl_vsil_webhdfs.cpp",
				"./gdal/port/cpl_vsil_unix_stdio_64.cpp",
				"./gdal/port/cpl_vsil_tar.cpp",
				"./gdal/port/cpl_vsil_swift.cpp",
				"./gdal/port/cpl_vsil_subfile.cpp",
				"./gdal/port/cpl_vsil_stdout.cpp",
				"./gdal/port/cpl_vsil_stdin.cpp",
				"./gdal/port/cpl_vsil_sparsefile.cpp",
				"./gdal/port/cpl_vsil_s3.cpp",
				"./gdal/port/cpl_vsil_plugin.cpp",
				"./gdal/port/cpl_vsil_oss.cpp",
				"./gdal/port/cpl_vsil_hdfs.cpp",
				"./gdal/port/cpl_vsil_gzip.cpp",
				"./gdal/port/cpl_vsil_gs.cpp",
				"./gdal/port/cpl_vsil_curl_streaming.cpp",
				"./gdal/port/cpl_vsil_curl.cpp",
				"./gdal/port/cpl_vsil_uploadonclose.cpp",
				"./gdal/port/cpl_vsil_crypt.cpp",
				"./gdal/port/cpl_vsil_cache.cpp",
				"./gdal/port/cpl_vsil_buffered_reader.cpp",
				"./gdal/port/cpl_vsil_az.cpp",
				"./gdal/port/cpl_vsil_adls.cpp",
				"./gdal/port/cpl_vsil_abstract_archive.cpp",
				"./gdal/port/cpl_vsil.cpp",
				"./gdal/port/cpl_vsi_mem.cpp",
				"./gdal/port/cpl_vsi_error.cpp",
				"./gdal/port/cpl_virtualmem.cpp",
				"./gdal/port/cpl_userfaultfd.cpp",
				"./gdal/port/cpl_time.cpp",
				"./gdal/port/cpl_swift.cpp",
				"./gdal/port/cpl_strtod.cpp",
				"./gdal/port/cpl_string.cpp",
				"./gdal/port/cpl_spawn.cpp",
				"./gdal/port/cpl_sha256.cpp",
				"./gdal/port/cpl_sha1.cpp",
				"./gdal/port/cpl_recode_stub.cpp",
				"./gdal/port/cpl_recode_iconv.cpp",
				"./gdal/port/cpl_recode.cpp",
				"./gdal/port/cpl_quad_tree.cpp",
				"./gdal/port/cpl_progress.cpp",
				"./gdal/port/cpl_path.cpp",
				#"./gdal/port/cpl_odbc.cpp",
				"./gdal/port/cpl_multiproc.cpp",
				"./gdal/port/cpl_minizip_zip.cpp",
				"./gdal/port/cpl_minizip_unzip.cpp",
				"./gdal/port/cpl_minizip_ioapi.cpp",
				"./gdal/port/cpl_minixml.cpp",
				"./gdal/port/cpl_md5.cpp",
				"./gdal/port/cpl_list.cpp",
				"./gdal/port/cpl_json_streaming_parser.cpp",
				"./gdal/port/cpl_json.cpp",
				"./gdal/port/cpl_http.cpp",
				"./gdal/port/cpl_hash_set.cpp",
				"./gdal/port/cpl_google_oauth2.cpp",
				"./gdal/port/cpl_google_cloud.cpp",
				"./gdal/port/cpl_getexecpath.cpp",
				"./gdal/port/cpl_findfile.cpp",
				"./gdal/port/cpl_error.cpp",
				"./gdal/port/cpl_csv.cpp",
				"./gdal/port/cpl_cpu_features.cpp",
				"./gdal/port/cpl_conv.cpp",
				"./gdal/port/cpl_base64.cpp",
				"./gdal/port/cpl_azure.cpp",
				"./gdal/port/cpl_aws.cpp",
				"./gdal/port/cpl_aws_win32.cpp",
				"./gdal/port/cpl_atomic_ops.cpp",
				"./gdal/port/cpl_alibaba_oss.cpp",
        "./gdal/port/cpl_vax.cpp",
        "./gdal/port/cpl_compressor.cpp",

				'<!@(python ../glob-files.py "./gdal/gnm/*.cpp")',
				'<!@(python ../glob-files.py "./gdal/frmts/jpeg/libjpeg/*.c")'
			],
			"include_dirs": [
				"./gdal/alg",
				"./gdal/apps",
				"./gdal/gcore",
				"./gdal/port",
				"./gdal/frmts",
				"./gdal/frmts/vrt",
				"./gdal/frmts/mem",
				"./gdal/frmts/derived",
				"./gdal/ogr",
				"./gdal/ogr/ogrsf_frmts",
				"./gdal/ogr/ogrsf_frmts/mem",
				"./gdal/ogr/ogrsf_frmts/geojson",
				"./gdal/frmts/jpeg/libjpeg",
				"./gdal/alg/marching_squares"
			],
			"dependencies": [
				'<@(gdal_format_gyps)'
			],
			"defines": [
				'<@(gdal_format_defs)'
			],
			"conditions": [
        ["runtime == 'electron'", {
          "include_dirs": [
    			  "./gdal/frmts/zlib"
          ]
        }],
				["OS == 'win'", {
					"sources": [
						"./gdal/port/cpl_odbc.cpp"
					],
					"link_settings": {
						"libraries": [
							"-lws2_32.lib",
							"-lodbccp32.lib"
						]
					}
				}]
			],
			"direct_dependent_settings": {
				"include_dirs": [
					"./gdal/alg",
					"./gdal/gcore",
					"./gdal/port",
					"./gdal/apps",
					"./gdal/ogr",
					"./gdal/ogr/ogrsf_frmts"
				],
				"conditions": [
					["OS == 'win'", {
						"include_dirs": ["arch/win"]
					}, {
						"include_dirs": ["arch/unix"]
					}],
					[ "OS == 'mac'", {
					    "libraries": [
							"-liconv"
						]
					}]
				],
				"defines": [
					"_LARGEFILE_SOURCE",
					"_FILE_OFFSET_BITS=64"
				]
			}
		}
	]
}
