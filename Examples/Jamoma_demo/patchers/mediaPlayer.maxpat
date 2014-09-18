{
	"patcher" : 	{
		"fileversion" : 1,
		"appversion" : 		{
			"major" : 6,
			"minor" : 1,
			"revision" : 8,
			"architecture" : "x86"
		}
,
		"rect" : [ 104.0, 122.0, 636.0, 660.0 ],
		"bglocked" : 0,
		"openinpresentation" : 0,
		"default_fontsize" : 12.0,
		"default_fontface" : 0,
		"default_fontname" : "Arial",
		"gridonopen" : 0,
		"gridsize" : [ 15.0, 15.0 ],
		"gridsnaponopen" : 0,
		"statusbarvisible" : 0,
		"toolbarvisible" : 1,
		"boxanimatetime" : 200,
		"imprint" : 0,
		"enablehscroll" : 1,
		"enablevscroll" : 1,
		"devicewidth" : 0.0,
		"description" : "",
		"digest" : "",
		"tags" : "",
		"showrootpatcherontab" : 0,
		"showontab" : 0,
		"boxes" : [ 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-1",
					"maxclass" : "newobj",
					"numinlets" : 0,
					"numoutlets" : 0,
					"patcher" : 					{
						"fileversion" : 1,
						"appversion" : 						{
							"major" : 6,
							"minor" : 1,
							"revision" : 8,
							"architecture" : "x86"
						}
,
						"rect" : [ 0.0, 26.0, 636.0, 634.0 ],
						"bglocked" : 0,
						"openinpresentation" : 1,
						"default_fontsize" : 12.0,
						"default_fontface" : 0,
						"default_fontname" : "Arial",
						"gridonopen" : 0,
						"gridsize" : [ 15.0, 15.0 ],
						"gridsnaponopen" : 0,
						"statusbarvisible" : 2,
						"toolbarvisible" : 1,
						"boxanimatetime" : 200,
						"imprint" : 0,
						"enablehscroll" : 1,
						"enablevscroll" : 1,
						"devicewidth" : 0.0,
						"description" : "",
						"digest" : "",
						"tags" : "",
						"showontab" : 1,
						"boxes" : [ 							{
								"box" : 								{
									"annotation" : "none",
									"args" : [ "video/track.2/brcosa" ],
									"id" : "obj-31",
									"maxclass" : "bpatcher",
									"name" : "gl_brcosa.view.maxpat",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 676.0, 203.0, 300.0, 70.0 ],
									"presentation" : 1,
									"presentation_rect" : [ 325.0, 95.0, 300.0, 70.0 ]
								}

							}
, 							{
								"box" : 								{
									"annotation" : "none",
									"args" : [ "video/track.2/plane" ],
									"id" : "obj-26",
									"lockeddragscroll" : 1,
									"maxclass" : "bpatcher",
									"name" : "gl_videoplane.view.maxpat",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 676.0, 304.0, 300.0, 140.0 ],
									"presentation" : 1,
									"presentation_rect" : [ 325.0, 181.0, 300.0, 140.0 ],
									"varname" : "gl.videoplane[1]"
								}

							}
, 							{
								"box" : 								{
									"annotation" : "Input from either a camera - video file - noise synthesis - or a real time screen capture.",
									"args" : [ "video/track.2/input" ],
									"id" : "obj-27",
									"maxclass" : "bpatcher",
									"name" : "input%.view.maxpat",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 676.0, 114.0, 300.0, 70.0 ],
									"presentation" : 1,
									"presentation_rect" : [ 325.0, 9.0, 300.0, 70.0 ]
								}

							}
, 							{
								"box" : 								{
									"annotation" : "none",
									"args" : [ "video/track.1/plane" ],
									"id" : "obj-21",
									"lockeddragscroll" : 1,
									"maxclass" : "bpatcher",
									"name" : "gl_videoplane.view.maxpat",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 361.5, 304.0, 300.0, 140.0 ],
									"presentation" : 1,
									"presentation_rect" : [ 10.0, 181.0, 300.0, 140.0 ],
									"varname" : "gl.videoplane"
								}

							}
, 							{
								"box" : 								{
									"annotation" : "none",
									"args" : [ "video/track.1/brcosa" ],
									"id" : "obj-22",
									"maxclass" : "bpatcher",
									"name" : "gl_brcosa.view.maxpat",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 361.5, 203.0, 300.0, 70.0 ],
									"presentation" : 1,
									"presentation_rect" : [ 10.0, 95.0, 300.0, 70.0 ]
								}

							}
, 							{
								"box" : 								{
									"annotation" : "Input from either a camera - video file - noise synthesis - or a real time screen capture.",
									"args" : [ "video/track.1/input" ],
									"id" : "obj-23",
									"maxclass" : "bpatcher",
									"name" : "input%.view.maxpat",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 361.5, 114.0, 300.0, 70.0 ],
									"presentation" : 1,
									"presentation_rect" : [ 10.0, 9.0, 300.0, 70.0 ]
								}

							}
, 							{
								"box" : 								{
									"annotation" : "none",
									"args" : [ "video/gl_render" ],
									"id" : "obj-24",
									"lockeddragscroll" : 1,
									"maxclass" : "bpatcher",
									"name" : "gl_render.view.maxpat",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 37.5, 114.0, 300.0, 175.0 ],
									"presentation" : 1,
									"presentation_rect" : [ 11.0, 426.0, 300.0, 175.0 ]
								}

							}
, 							{
								"box" : 								{
									"annotation" : "none",
									"args" : [ "video/window%" ],
									"id" : "obj-25",
									"maxclass" : "bpatcher",
									"name" : "window%.view.maxpat",
									"numinlets" : 0,
									"numoutlets" : 0,
									"patching_rect" : [ 37.5, 314.0, 300.0, 105.0 ],
									"presentation" : 1,
									"presentation_rect" : [ 321.0, 426.0, 300.0, 105.0 ]
								}

							}
, 							{
								"box" : 								{
									"bgcolor" : [ 0.20073, 0.20073, 0.20073, 1.0 ],
									"id" : "obj-28",
									"maxclass" : "panel",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 102.0, 480.0, 128.0, 128.0 ],
									"presentation" : 1,
									"presentation_rect" : [ 2.0, 3.0, 314.0, 398.0 ]
								}

							}
, 							{
								"box" : 								{
									"bgcolor" : [ 0.20073, 0.20073, 0.20073, 1.0 ],
									"id" : "obj-29",
									"maxclass" : "panel",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 600.0, 593.0, 128.0, 128.0 ],
									"presentation" : 1,
									"presentation_rect" : [ 319.0, 3.0, 314.0, 398.0 ]
								}

							}
, 							{
								"box" : 								{
									"bgcolor" : [ 0.20073, 0.20073, 0.20073, 1.0 ],
									"id" : "obj-30",
									"maxclass" : "panel",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 345.0, 703.0, 128.0, 128.0 ],
									"presentation" : 1,
									"presentation_rect" : [ 2.0, 417.5, 631.0, 192.5 ]
								}

							}
 ],
						"lines" : [  ]
					}
,
					"patching_rect" : [ 264.0, 205.0, 95.0, 20.0 ],
					"saved_object_attributes" : 					{
						"default_fontface" : 0,
						"default_fontname" : "Arial",
						"default_fontsize" : 12.0,
						"description" : "",
						"digest" : "",
						"fontface" : 0,
						"fontname" : "Arial",
						"fontsize" : 12.0,
						"globalpatchername" : "",
						"tags" : ""
					}
,
					"text" : "p \"Video tracks\""
				}

			}
, 			{
				"box" : 				{
					"annotation" : "none",
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-10",
					"maxclass" : "newobj",
					"numinlets" : 0,
					"numoutlets" : 0,
					"patcher" : 					{
						"fileversion" : 1,
						"appversion" : 						{
							"major" : 6,
							"minor" : 1,
							"revision" : 8,
							"architecture" : "x86"
						}
,
						"rect" : [ 104.0, 148.0, 636.0, 634.0 ],
						"bglocked" : 0,
						"openinpresentation" : 1,
						"default_fontsize" : 12.0,
						"default_fontface" : 0,
						"default_fontname" : "Arial",
						"gridonopen" : 0,
						"gridsize" : [ 15.0, 15.0 ],
						"gridsnaponopen" : 0,
						"statusbarvisible" : 2,
						"toolbarvisible" : 1,
						"boxanimatetime" : 200,
						"imprint" : 0,
						"enablehscroll" : 1,
						"enablevscroll" : 1,
						"devicewidth" : 0.0,
						"description" : "",
						"digest" : "",
						"tags" : "",
						"showontab" : 1,
						"boxes" : [ 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-35",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 1143.0, 413.0, 163.0, 18.0 ],
									"text" : "model:address /mediaPlayer"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-33",
									"maxclass" : "newobj",
									"numinlets" : 0,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 1143.0, 373.0, 139.0, 20.0 ],
									"text" : "j.initialized /mediaPlayer"
								}

							}
, 							{
								"box" : 								{
									"annotation" : "balance amount",
									"id" : "obj-17",
									"maxclass" : "live.slider",
									"numinlets" : 1,
									"numoutlets" : 2,
									"orientation" : 1,
									"outlettype" : [ "", "float" ],
									"parameter_enable" : 1,
									"patching_rect" : [ 794.0, 610.5, 74.0, 27.0 ],
									"presentation" : 1,
									"presentation_rect" : [ 166.0, 413.5, 300.0, 27.0 ],
									"saved_attribute_attributes" : 									{
										"valueof" : 										{
											"parameter_longname" : "live.slider",
											"parameter_shortname" : "Crossfade",
											"parameter_type" : 0,
											"parameter_mmax" : 1.0,
											"parameter_initial" : [ 0.5 ],
											"parameter_unitstyle" : 1
										}

									}
,
									"shownumber" : 0,
									"varname" : "live.slider"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-16",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 4,
									"outlettype" : [ "", "", "", "" ],
									"patching_rect" : [ 883.0, 614.0, 164.0, 20.0 ],
									"text" : "j.remote audio/crossfade/mix"
								}

							}
, 							{
								"box" : 								{
									"annotation" : "none",
									"args" : [ "audio/output" ],
									"id" : "obj-15",
									"maxclass" : "bpatcher",
									"name" : "output~.view.maxpat",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 425.0, 468.0, 300.0, 175.0 ],
									"presentation" : 1,
									"presentation_rect" : [ 166.0, 447.0, 300.0, 175.0 ]
								}

							}
, 							{
								"box" : 								{
									"annotation" : "none",
									"args" : [ "audio/track.2/echo" ],
									"id" : "obj-14",
									"maxclass" : "bpatcher",
									"name" : "echo~.view.maxpat",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 681.0, 308.0, 300.0, 105.0 ],
									"presentation" : 1,
									"presentation_rect" : [ 325.0, 289.0, 300.0, 105.0 ]
								}

							}
, 							{
								"box" : 								{
									"annotation" : "none",
									"args" : [ "audio/track.1/echo" ],
									"id" : "obj-13",
									"maxclass" : "bpatcher",
									"name" : "echo~.view.maxpat",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 324.0, 308.0, 300.0, 105.0 ],
									"presentation" : 1,
									"presentation_rect" : [ 8.0, 289.0, 300.0, 105.0 ]
								}

							}
, 							{
								"box" : 								{
									"annotation" : "none",
									"args" : [ "audio/track.2/eq" ],
									"id" : "obj-12",
									"maxclass" : "bpatcher",
									"name" : "equalizer~_wide.view.maxpat",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 681.0, 192.0, 300.0, 105.0 ],
									"presentation" : 1,
									"presentation_rect" : [ 325.0, 183.0, 300.0, 105.0 ]
								}

							}
, 							{
								"box" : 								{
									"annotation" : "none",
									"args" : [ "audio/track.1/eq" ],
									"id" : "obj-10",
									"maxclass" : "bpatcher",
									"name" : "equalizer~_wide.view.maxpat",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 324.0, 192.0, 300.0, 105.0 ],
									"presentation" : 1,
									"presentation_rect" : [ 8.0, 183.0, 300.0, 105.0 ]
								}

							}
, 							{
								"box" : 								{
									"annotation" : "none",
									"args" : [ "audio/track.2/input" ],
									"id" : "obj-4",
									"maxclass" : "bpatcher",
									"name" : "input~.view.maxpat",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 681.0, 7.0, 300.0, 175.0 ],
									"presentation" : 1,
									"presentation_rect" : [ 325.0, 7.0, 300.0, 175.0 ]
								}

							}
, 							{
								"box" : 								{
									"annotation" : "none",
									"args" : [ "audio/track.1/input" ],
									"id" : "obj-3",
									"maxclass" : "bpatcher",
									"name" : "input~.view.maxpat",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 324.0, 7.0, 300.0, 175.0 ],
									"presentation" : 1,
									"presentation_rect" : [ 8.0, 7.0, 300.0, 175.0 ]
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-20",
									"maxclass" : "panel",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 124.0, 584.0, 128.0, 128.0 ],
									"presentation" : 1,
									"presentation_rect" : [ 166.0, 410.0, 300.0, 34.0 ]
								}

							}
, 							{
								"box" : 								{
									"bgcolor" : [ 0.20073, 0.20073, 0.20073, 1.0 ],
									"id" : "obj-11",
									"maxclass" : "panel",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 149.0, 668.0, 128.0, 128.0 ],
									"presentation" : 1,
									"presentation_rect" : [ 1.0, 1.0, 313.0, 398.0 ]
								}

							}
, 							{
								"box" : 								{
									"bgcolor" : [ 0.20073, 0.20073, 0.20073, 1.0 ],
									"id" : "obj-18",
									"maxclass" : "panel",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 117.0, 45.0, 128.0, 128.0 ],
									"presentation" : 1,
									"presentation_rect" : [ 318.0, 1.0, 313.0, 398.0 ]
								}

							}
, 							{
								"box" : 								{
									"bgcolor" : [ 0.20073, 0.20073, 0.20073, 1.0 ],
									"id" : "obj-19",
									"maxclass" : "panel",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 167.0, 408.0, 128.0, 128.0 ],
									"presentation" : 1,
									"presentation_rect" : [ 159.0, 405.0, 313.0, 221.0 ]
								}

							}
 ],
						"lines" : [ 							{
								"patchline" : 								{
									"destination" : [ "obj-17", 0 ],
									"disabled" : 0,
									"hidden" : 0,
									"source" : [ "obj-16", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-16", 0 ],
									"disabled" : 0,
									"hidden" : 0,
									"source" : [ "obj-17", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-35", 0 ],
									"disabled" : 0,
									"hidden" : 0,
									"source" : [ "obj-33", 0 ]
								}

							}
 ]
					}
,
					"patching_rect" : [ 181.0, 150.0, 95.0, 20.0 ],
					"saved_object_attributes" : 					{
						"default_fontface" : 0,
						"default_fontname" : "Arial",
						"default_fontsize" : 12.0,
						"description" : "",
						"digest" : "",
						"fontface" : 0,
						"fontname" : "Arial",
						"fontsize" : 12.0,
						"globalpatchername" : "",
						"tags" : ""
					}
,
					"text" : "p \"Audio tracks\""
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-9",
					"maxclass" : "newobj",
					"numinlets" : 0,
					"numoutlets" : 0,
					"patcher" : 					{
						"fileversion" : 1,
						"appversion" : 						{
							"major" : 6,
							"minor" : 1,
							"revision" : 8,
							"architecture" : "x86"
						}
,
						"rect" : [ 0.0, 26.0, 636.0, 634.0 ],
						"bglocked" : 0,
						"openinpresentation" : 0,
						"default_fontsize" : 12.0,
						"default_fontface" : 0,
						"default_fontname" : "Arial",
						"gridonopen" : 0,
						"gridsize" : [ 15.0, 15.0 ],
						"gridsnaponopen" : 0,
						"statusbarvisible" : 2,
						"toolbarvisible" : 1,
						"boxanimatetime" : 200,
						"imprint" : 0,
						"enablehscroll" : 1,
						"enablevscroll" : 1,
						"devicewidth" : 0.0,
						"description" : "",
						"digest" : "",
						"tags" : "",
						"showontab" : 1,
						"boxes" : [ 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-7",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 14.0, 36.0, 76.0, 20.0 ],
									"text" : "loadmess 1"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 18.0,
									"id" : "obj-1",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 14.0, 62.5, 234.0, 27.0 ],
									"text" : "MinuitInterface mediaPlayer"
								}

							}
, 							{
								"box" : 								{
									"annotation" : "none",
									"fontname" : "Arial",
									"fontsize" : 18.0,
									"id" : "obj-3",
									"maxclass" : "newobj",
									"numinlets" : 0,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patcher" : 									{
										"fileversion" : 1,
										"appversion" : 										{
											"major" : 6,
											"minor" : 1,
											"revision" : 8,
											"architecture" : "x86"
										}
,
										"rect" : [ 786.0, 429.0, 348.0, 242.0 ],
										"bglocked" : 0,
										"openinpresentation" : 0,
										"default_fontsize" : 12.0,
										"default_fontface" : 0,
										"default_fontname" : "Arial",
										"gridonopen" : 0,
										"gridsize" : [ 15.0, 15.0 ],
										"gridsnaponopen" : 0,
										"statusbarvisible" : 2,
										"toolbarvisible" : 1,
										"boxanimatetime" : 200,
										"imprint" : 0,
										"enablehscroll" : 1,
										"enablevscroll" : 1,
										"devicewidth" : 0.0,
										"description" : "",
										"digest" : "",
										"tags" : "",
										"boxes" : [ 											{
												"box" : 												{
													"comment" : "",
													"id" : "obj-6",
													"maxclass" : "outlet",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 254.0, 175.0, 25.0, 25.0 ]
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-5",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 2,
													"outlettype" : [ "", "" ],
													"patching_rect" : [ 254.0, 140.0, 49.0, 20.0 ],
													"text" : "j.model"
												}

											}
, 											{
												"box" : 												{
													"annotation" : "none",
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-3",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 131.0, 89.0, 105.0, 20.0 ],
													"text" : "videotrack track.2"
												}

											}
, 											{
												"box" : 												{
													"annotation" : "Output to an OpenGL window",
													"fontname" : "Verdana",
													"fontsize" : 10.0,
													"id" : "obj-4",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 2,
													"outlettype" : [ "bang", "" ],
													"patching_rect" : [ 16.0, 140.0, 93.0, 19.0 ],
													"text" : "window%.model"
												}

											}
, 											{
												"box" : 												{
													"annotation" : "create a GL render context.",
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-1",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 2,
													"outlettype" : [ "", "" ],
													"patching_rect" : [ 16.0, 25.0, 97.0, 20.0 ],
													"text" : "gl_render.model"
												}

											}
, 											{
												"box" : 												{
													"annotation" : "none",
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-2",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 16.0, 89.0, 105.0, 20.0 ],
													"text" : "videotrack track.1"
												}

											}
 ],
										"lines" : [ 											{
												"patchline" : 												{
													"destination" : [ "obj-2", 0 ],
													"disabled" : 0,
													"hidden" : 0,
													"source" : [ "obj-1", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-3", 0 ],
													"disabled" : 0,
													"hidden" : 0,
													"midpoints" : [ 25.5, 66.5, 140.5, 66.5 ],
													"source" : [ "obj-1", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-6", 0 ],
													"disabled" : 0,
													"hidden" : 0,
													"source" : [ "obj-5", 0 ]
												}

											}
 ]
									}
,
									"patching_rect" : [ 14.0, 149.0, 70.0, 27.0 ],
									"saved_object_attributes" : 									{
										"default_fontface" : 0,
										"default_fontname" : "Arial",
										"default_fontsize" : 12.0,
										"description" : "",
										"digest" : "",
										"fontface" : 0,
										"fontname" : "Arial",
										"fontsize" : 12.0,
										"globalpatchername" : "",
										"tags" : ""
									}
,
									"text" : "p video"
								}

							}
, 							{
								"box" : 								{
									"annotation" : "none",
									"fontname" : "Arial",
									"fontsize" : 18.0,
									"id" : "obj-2",
									"maxclass" : "newobj",
									"numinlets" : 0,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patcher" : 									{
										"fileversion" : 1,
										"appversion" : 										{
											"major" : 6,
											"minor" : 1,
											"revision" : 8,
											"architecture" : "x86"
										}
,
										"rect" : [ 2667.0, 479.0, 404.0, 268.0 ],
										"bglocked" : 0,
										"openinpresentation" : 0,
										"default_fontsize" : 12.0,
										"default_fontface" : 0,
										"default_fontname" : "Arial",
										"gridonopen" : 0,
										"gridsize" : [ 15.0, 15.0 ],
										"gridsnaponopen" : 0,
										"statusbarvisible" : 2,
										"toolbarvisible" : 1,
										"boxanimatetime" : 200,
										"imprint" : 0,
										"enablehscroll" : 1,
										"enablevscroll" : 1,
										"devicewidth" : 0.0,
										"description" : "",
										"digest" : "",
										"tags" : "",
										"boxes" : [ 											{
												"box" : 												{
													"comment" : "",
													"id" : "obj-4",
													"maxclass" : "outlet",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 326.0, 223.0, 25.0, 25.0 ]
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-3",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 2,
													"outlettype" : [ "", "" ],
													"patching_rect" : [ 326.0, 158.0, 49.0, 20.0 ],
													"text" : "j.model"
												}

											}
, 											{
												"box" : 												{
													"annotation" : "none",
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-2",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 3,
													"outlettype" : [ "signal", "signal", "" ],
													"patching_rect" : [ 136.0, 57.0, 105.0, 20.0 ],
													"text" : "audiotrack track.2"
												}

											}
, 											{
												"box" : 												{
													"annotation" : "none",
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-1",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 3,
													"outlettype" : [ "signal", "signal", "" ],
													"patching_rect" : [ 50.0, 25.0, 105.0, 20.0 ],
													"text" : "audiotrack track.1"
												}

											}
, 											{
												"box" : 												{
													"annotation" : "A generic audio output model with built in master saturation, limiter, and recording abilities.",
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-8",
													"maxclass" : "newobj",
													"numinlets" : 2,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patching_rect" : [ 50.0, 223.0, 124.0, 20.0 ],
													"text" : "output~.model output"
												}

											}
, 											{
												"box" : 												{
													"annotation" : "none",
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-6",
													"maxclass" : "newobj",
													"numinlets" : 4,
													"numoutlets" : 3,
													"outlettype" : [ "signal", "signal", "" ],
													"patcher" : 													{
														"fileversion" : 1,
														"appversion" : 														{
															"major" : 6,
															"minor" : 1,
															"revision" : 8,
															"architecture" : "x86"
														}
,
														"rect" : [ 2232.0, 420.0, 640.0, 480.0 ],
														"bglocked" : 0,
														"openinpresentation" : 0,
														"default_fontsize" : 12.0,
														"default_fontface" : 0,
														"default_fontname" : "Arial",
														"gridonopen" : 0,
														"gridsize" : [ 15.0, 15.0 ],
														"gridsnaponopen" : 0,
														"statusbarvisible" : 2,
														"toolbarvisible" : 1,
														"boxanimatetime" : 200,
														"imprint" : 0,
														"enablehscroll" : 1,
														"enablevscroll" : 1,
														"devicewidth" : 0.0,
														"description" : "",
														"digest" : "",
														"tags" : "",
														"boxes" : [ 															{
																"box" : 																{
																	"fontname" : "Arial",
																	"fontsize" : 12.0,
																	"id" : "obj-13",
																	"maxclass" : "message",
																	"numinlets" : 2,
																	"numoutlets" : 1,
																	"outlettype" : [ "" ],
																	"patching_rect" : [ 373.5, 195.0, 69.0, 18.0 ],
																	"text" : "position $1"
																}

															}
, 															{
																"box" : 																{
																	"fontname" : "Arial",
																	"fontsize" : 12.0,
																	"id" : "obj-10",
																	"linecount" : 7,
																	"maxclass" : "newobj",
																	"numinlets" : 1,
																	"numoutlets" : 2,
																	"outlettype" : [ "", "" ],
																	"patching_rect" : [ 373.5, 66.0, 114.0, 100.0 ],
																	"text" : "j.parameter mix @type decimal @range 0 1 @clipmode both @description \"balance amount\" @default 0.5"
																}

															}
, 															{
																"box" : 																{
																	"comment" : "",
																	"id" : "obj-9",
																	"maxclass" : "outlet",
																	"numinlets" : 1,
																	"numoutlets" : 0,
																	"patching_rect" : [ 423.0, 313.0, 25.0, 25.0 ]
																}

															}
, 															{
																"box" : 																{
																	"fontname" : "Arial",
																	"fontsize" : 12.0,
																	"id" : "obj-8",
																	"maxclass" : "newobj",
																	"numinlets" : 1,
																	"numoutlets" : 2,
																	"outlettype" : [ "", "" ],
																	"patching_rect" : [ 423.0, 246.0, 49.0, 20.0 ],
																	"text" : "j.model"
																}

															}
, 															{
																"box" : 																{
																	"comment" : "",
																	"id" : "obj-7",
																	"maxclass" : "outlet",
																	"numinlets" : 1,
																	"numoutlets" : 0,
																	"patching_rect" : [ 327.0, 313.0, 25.0, 25.0 ]
																}

															}
, 															{
																"box" : 																{
																	"comment" : "",
																	"id" : "obj-6",
																	"maxclass" : "outlet",
																	"numinlets" : 1,
																	"numoutlets" : 0,
																	"patching_rect" : [ 186.0, 313.0, 25.0, 25.0 ]
																}

															}
, 															{
																"box" : 																{
																	"fontname" : "Arial",
																	"fontsize" : 12.0,
																	"id" : "obj-5",
																	"maxclass" : "newobj",
																	"numinlets" : 5,
																	"numoutlets" : 3,
																	"outlettype" : [ "signal", "signal", "" ],
																	"patching_rect" : [ 186.0, 246.0, 207.0, 20.0 ],
																	"text" : "j.xfade~ 2"
																}

															}
, 															{
																"box" : 																{
																	"comment" : "",
																	"id" : "obj-4",
																	"maxclass" : "inlet",
																	"numinlets" : 0,
																	"numoutlets" : 1,
																	"outlettype" : [ "signal" ],
																	"patching_rect" : [ 327.0, 174.0, 25.0, 25.0 ]
																}

															}
, 															{
																"box" : 																{
																	"comment" : "",
																	"id" : "obj-3",
																	"maxclass" : "inlet",
																	"numinlets" : 0,
																	"numoutlets" : 1,
																	"outlettype" : [ "signal" ],
																	"patching_rect" : [ 280.0, 174.0, 25.0, 25.0 ]
																}

															}
, 															{
																"box" : 																{
																	"comment" : "",
																	"id" : "obj-2",
																	"maxclass" : "inlet",
																	"numinlets" : 0,
																	"numoutlets" : 1,
																	"outlettype" : [ "signal" ],
																	"patching_rect" : [ 233.0, 174.0, 25.0, 25.0 ]
																}

															}
, 															{
																"box" : 																{
																	"comment" : "",
																	"id" : "obj-1",
																	"maxclass" : "inlet",
																	"numinlets" : 0,
																	"numoutlets" : 1,
																	"outlettype" : [ "signal" ],
																	"patching_rect" : [ 186.0, 174.0, 25.0, 25.0 ]
																}

															}
 ],
														"lines" : [ 															{
																"patchline" : 																{
																	"destination" : [ "obj-5", 0 ],
																	"disabled" : 0,
																	"hidden" : 0,
																	"source" : [ "obj-1", 0 ]
																}

															}
, 															{
																"patchline" : 																{
																	"destination" : [ "obj-13", 0 ],
																	"disabled" : 0,
																	"hidden" : 0,
																	"source" : [ "obj-10", 0 ]
																}

															}
, 															{
																"patchline" : 																{
																	"destination" : [ "obj-5", 4 ],
																	"disabled" : 0,
																	"hidden" : 0,
																	"midpoints" : [ 383.0, 237.0, 383.5, 237.0 ],
																	"source" : [ "obj-13", 0 ]
																}

															}
, 															{
																"patchline" : 																{
																	"destination" : [ "obj-5", 1 ],
																	"disabled" : 0,
																	"hidden" : 0,
																	"source" : [ "obj-2", 0 ]
																}

															}
, 															{
																"patchline" : 																{
																	"destination" : [ "obj-5", 2 ],
																	"disabled" : 0,
																	"hidden" : 0,
																	"source" : [ "obj-3", 0 ]
																}

															}
, 															{
																"patchline" : 																{
																	"destination" : [ "obj-5", 3 ],
																	"disabled" : 0,
																	"hidden" : 0,
																	"source" : [ "obj-4", 0 ]
																}

															}
, 															{
																"patchline" : 																{
																	"destination" : [ "obj-6", 0 ],
																	"disabled" : 0,
																	"hidden" : 0,
																	"source" : [ "obj-5", 0 ]
																}

															}
, 															{
																"patchline" : 																{
																	"destination" : [ "obj-7", 0 ],
																	"disabled" : 0,
																	"hidden" : 0,
																	"source" : [ "obj-5", 1 ]
																}

															}
, 															{
																"patchline" : 																{
																	"destination" : [ "obj-9", 0 ],
																	"disabled" : 0,
																	"hidden" : 0,
																	"source" : [ "obj-8", 0 ]
																}

															}
 ]
													}
,
													"patching_rect" : [ 50.0, 158.0, 148.0, 20.0 ],
													"saved_object_attributes" : 													{
														"default_fontface" : 0,
														"default_fontname" : "Arial",
														"default_fontsize" : 12.0,
														"description" : "",
														"digest" : "",
														"fontface" : 0,
														"fontname" : "Arial",
														"fontsize" : 12.0,
														"globalpatchername" : "",
														"tags" : ""
													}
,
													"text" : "p crossfade"
												}

											}
 ],
										"lines" : [ 											{
												"patchline" : 												{
													"destination" : [ "obj-6", 1 ],
													"disabled" : 0,
													"hidden" : 0,
													"source" : [ "obj-1", 1 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-6", 0 ],
													"disabled" : 0,
													"hidden" : 0,
													"source" : [ "obj-1", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-6", 3 ],
													"disabled" : 0,
													"hidden" : 0,
													"source" : [ "obj-2", 1 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-6", 2 ],
													"disabled" : 0,
													"hidden" : 0,
													"source" : [ "obj-2", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-4", 0 ],
													"disabled" : 0,
													"hidden" : 0,
													"source" : [ "obj-3", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-8", 1 ],
													"disabled" : 0,
													"hidden" : 0,
													"source" : [ "obj-6", 1 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-8", 0 ],
													"disabled" : 0,
													"hidden" : 0,
													"source" : [ "obj-6", 0 ]
												}

											}
 ]
									}
,
									"patching_rect" : [ 14.0, 113.0, 71.0, 27.0 ],
									"saved_object_attributes" : 									{
										"default_fontface" : 0,
										"default_fontname" : "Arial",
										"default_fontsize" : 12.0,
										"description" : "",
										"digest" : "",
										"fontface" : 0,
										"fontname" : "Arial",
										"fontsize" : 12.0,
										"globalpatchername" : "",
										"tags" : ""
									}
,
									"text" : "p audio"
								}

							}
 ],
						"lines" : [ 							{
								"patchline" : 								{
									"destination" : [ "obj-1", 0 ],
									"disabled" : 0,
									"hidden" : 0,
									"source" : [ "obj-7", 0 ]
								}

							}
 ]
					}
,
					"patching_rect" : [ 307.0, 150.0, 58.0, 20.0 ],
					"saved_object_attributes" : 					{
						"default_fontface" : 0,
						"default_fontname" : "Arial",
						"default_fontsize" : 12.0,
						"description" : "",
						"digest" : "",
						"fontface" : 0,
						"fontname" : "Arial",
						"fontsize" : 12.0,
						"globalpatchername" : "",
						"tags" : ""
					}
,
					"text" : "p Engine"
				}

			}
 ],
		"lines" : [  ],
		"parameters" : 		{
			"obj-10::obj-3::obj-29::obj-48" : [ "live.menu[18]", "live.menu", 0 ],
			"obj-10::obj-14::obj-3::obj-12" : [ "Lookahead[7]", "Lookahead", 0 ],
			"obj-10::obj-3::obj-1::obj-30" : [ "live.menu[16]", "live.menu", 0 ],
			"obj-10::obj-14::obj-3::obj-45" : [ "live.text[62]", "live.text", 0 ],
			"obj-10::obj-3::obj-4" : [ "live.text[42]", "live.text[1]", 0 ],
			"obj-10::obj-14::obj-18" : [ "live.text[61]", "live.text[3]", 0 ],
			"obj-10::obj-3::obj-59" : [ "live.text[39]", "live.text[3]", 0 ],
			"obj-10::obj-15::obj-107" : [ "live.dial[15]", "Preamp", 0 ],
			"obj-10::obj-3::obj-62" : [ "live.text[38]", "live.text[3]", 0 ],
			"obj-10::obj-4::obj-74" : [ "live.dial[10]", "Transpose", 0 ],
			"obj-10::obj-15::obj-104" : [ "live.dial[17]", "Depth", 0 ],
			"obj-10::obj-4::obj-1::obj-13" : [ "Release[5]", "Release", 0 ],
			"obj-10::obj-10::obj-5::obj-6" : [ "live.numbox[44]", "live.numbox[4]", 0 ],
			"obj-10::obj-15::obj-111" : [ "live.numbox[35]", "live.numbox[2]", 0 ],
			"obj-10::obj-4::obj-1::obj-45" : [ "live.text[43]", "live.text", 0 ],
			"obj-10::obj-10::obj-5::obj-27" : [ "live.text[51]", "live.text[2]", 0 ],
			"obj-10::obj-4::obj-58" : [ "live.text[48]", "live.text[4]", 0 ],
			"obj-10::obj-12::obj-5::obj-7" : [ "live.numbox[41]", "live.numbox", 0 ],
			"obj-10::obj-12::obj-5::obj-6" : [ "live.numbox[40]", "live.numbox[4]", 0 ],
			"obj-10::obj-13::obj-3::obj-30" : [ "live.menu[26]", "live.menu", 0 ],
			"obj-10::obj-13::obj-17" : [ "live.menu[25]", "live.menu[1]", 0 ],
			"obj-10::obj-13::obj-21" : [ "live.menu[24]", "live.menu[2]", 0 ],
			"obj-10::obj-3::obj-1::obj-15" : [ "Postamp[4]", "Postamp", 0 ],
			"obj-10::obj-14::obj-3::obj-42" : [ "live.text[63]", "live.text", 0 ],
			"obj-10::obj-3::obj-1::obj-27" : [ "Threshold[4]", "Threshold", 0 ],
			"obj-10::obj-14::obj-25" : [ "Feedback[3]", "Feedback", 0 ],
			"obj-10::obj-3::obj-47" : [ "live.numbox[19]", "live.numbox", 0 ],
			"obj-10::obj-14::obj-21" : [ "live.menu[27]", "live.menu[2]", 0 ],
			"obj-10::obj-3::obj-82" : [ "pan[3]", "Pan", 0 ],
			"obj-10::obj-4::obj-4" : [ "live.text[50]", "live.text[1]", 0 ],
			"obj-10::obj-15::obj-98" : [ "live.menu[31]", "live.menu", 0 ],
			"obj-10::obj-3::obj-58" : [ "live.text[40]", "live.text[4]", 0 ],
			"obj-10::obj-4::obj-110" : [ "live.numbox[26]", "live.numbox[1]", 0 ],
			"obj-10::obj-15::obj-108" : [ "live.text[65]", "live.text[4]", 0 ],
			"obj-10::obj-4::obj-1::obj-42" : [ "live.text[44]", "live.text", 0 ],
			"obj-10::obj-10::obj-5::obj-17" : [ "live.numbox[43]", "live.numbox", 0 ],
			"obj-10::obj-15::obj-82" : [ "pan[5]", "Pan", 0 ],
			"obj-10::obj-4::obj-1::obj-12" : [ "Lookahead[5]", "Lookahead", 0 ],
			"obj-10::obj-10::obj-5::obj-19" : [ "live.numbox[42]", "live.numbox", 0 ],
			"obj-10::obj-4::obj-24" : [ "Master Gain[4]", "Master Gain", 0 ],
			"obj-10::obj-12::obj-5::obj-19" : [ "live.numbox[38]", "live.numbox", 0 ],
			"obj-10::obj-13::obj-3::obj-27" : [ "Threshold[6]", "Threshold", 0 ],
			"obj-10::obj-13::obj-3::obj-15" : [ "Postamp[6]", "Postamp", 0 ],
			"obj-10::obj-13::obj-25" : [ "Feedback[2]", "Feedback", 0 ],
			"obj-10::obj-14::obj-3::obj-5" : [ "Preamp[7]", "Preamp", 0 ],
			"obj-10::obj-3::obj-1::obj-42" : [ "live.text[36]", "live.text", 0 ],
			"obj-10::obj-14::obj-3::obj-6" : [ "live.text[64]", "live.text", 0 ],
			"obj-10::obj-3::obj-1::obj-45" : [ "live.text[35]", "live.text", 0 ],
			"obj-10::obj-14::obj-17" : [ "live.menu[28]", "live.menu[1]", 0 ],
			"obj-10::obj-3::obj-48" : [ "live.menu[17]", "live.menu", 0 ],
			"obj-10::obj-15::obj-99" : [ "live.menu[30]", "live.menu[2]", 0 ],
			"obj-10::obj-3::obj-108" : [ "live.text[41]", "live.text[4]", 0 ],
			"obj-10::obj-4::obj-48" : [ "live.menu[20]", "live.menu", 0 ],
			"obj-10::obj-15::obj-100" : [ "live.text[69]", "live.text[1]", 0 ],
			"obj-10::obj-3::obj-24" : [ "Master Gain[3]", "Master Gain", 0 ],
			"obj-10::obj-4::obj-45" : [ "live.numbox[24]", "live.numbox", 0 ],
			"obj-10::obj-15::obj-102" : [ "live.text[67]", "live.text", 0 ],
			"obj-10::obj-4::obj-1::obj-6" : [ "live.text[45]", "live.text", 0 ],
			"obj-10::obj-10::obj-5::obj-45" : [ "live.text[73]", "live.text", 0 ],
			"obj-10::obj-4::obj-1::obj-5" : [ "Preamp[5]", "Preamp", 0 ],
			"obj-10::obj-10::obj-5::obj-9" : [ "live.menu[33]", "live.menu", 0 ],
			"obj-10::obj-12::obj-5::obj-9" : [ "live.menu[32]", "live.menu", 0 ],
			"obj-10::obj-13::obj-3::obj-13" : [ "Release[6]", "Release", 0 ],
			"obj-10::obj-13::obj-3::obj-45" : [ "live.text[58]", "live.text", 0 ],
			"obj-10::obj-13::obj-23" : [ "live.dial[12]", "Delay L", 0 ],
			"obj-10::obj-14::obj-3::obj-30" : [ "live.menu[29]", "live.menu", 0 ],
			"obj-10::obj-3::obj-1::obj-12" : [ "Lookahead[4]", "Lookahead", 0 ],
			"obj-10::obj-14::obj-3::obj-27" : [ "Threshold[7]", "Threshold", 0 ],
			"obj-10::obj-3::obj-1::obj-13" : [ "Release[4]", "Release", 0 ],
			"obj-10::obj-14::obj-23" : [ "live.dial[14]", "Delay L", 0 ],
			"obj-10::obj-3::obj-74" : [ "live.dial[9]", "Transpose", 0 ],
			"obj-10::obj-15::obj-110" : [ "live.numbox[36]", "live.numbox[1]", 0 ],
			"obj-10::obj-3::obj-45" : [ "live.numbox[20]", "live.numbox", 0 ],
			"obj-10::obj-4::obj-62" : [ "live.text[46]", "live.text[3]", 0 ],
			"obj-10::obj-15::obj-90" : [ "live.text[68]", "live.text[1]", 0 ],
			"obj-10::obj-4::obj-29::obj-48" : [ "live.menu[21]", "live.menu", 0 ],
			"obj-10::obj-4::obj-82" : [ "pan[4]", "Pan", 0 ],
			"obj-10::obj-15::obj-106" : [ "live.dial[16]", "Release", 0 ],
			"obj-10::obj-4::obj-1::obj-30" : [ "live.menu[19]", "live.menu", 0 ],
			"obj-10::obj-10::obj-5::obj-7" : [ "live.numbox[45]", "live.numbox", 0 ],
			"obj-10::obj-4::obj-111" : [ "live.numbox[25]", "live.numbox[2]", 0 ],
			"obj-10::obj-12::obj-5::obj-17" : [ "live.numbox[39]", "live.numbox", 0 ],
			"obj-10::obj-17" : [ "live.slider", "Crossfade", 0 ],
			"obj-10::obj-12::obj-5::obj-46" : [ "live.text[71]", "live.text", 0 ],
			"obj-10::obj-13::obj-3::obj-42" : [ "live.text[59]", "live.text", 0 ],
			"obj-10::obj-13::obj-3::obj-12" : [ "Lookahead[6]", "Lookahead", 0 ],
			"obj-10::obj-13::obj-18" : [ "live.text[57]", "live.text[3]", 0 ],
			"obj-10::obj-14::obj-3::obj-15" : [ "Postamp[7]", "Postamp", 0 ],
			"obj-10::obj-3::obj-1::obj-5" : [ "Preamp[4]", "Preamp", 0 ],
			"obj-10::obj-14::obj-3::obj-13" : [ "Release[7]", "Release", 0 ],
			"obj-10::obj-3::obj-1::obj-6" : [ "live.text[37]", "live.text", 0 ],
			"obj-10::obj-14::obj-24" : [ "live.dial[13]", "Delay R", 0 ],
			"obj-10::obj-3::obj-110" : [ "live.numbox[22]", "live.numbox[1]", 0 ],
			"obj-10::obj-15::obj-103" : [ "live.text[66]", "live.text", 0 ],
			"obj-10::obj-3::obj-111" : [ "live.numbox[21]", "live.numbox[2]", 0 ],
			"obj-10::obj-4::obj-47" : [ "live.numbox[23]", "live.numbox", 0 ],
			"obj-10::obj-15::obj-53" : [ "Master Gain[5]", "Master Gain", 0 ],
			"obj-10::obj-4::obj-1::obj-27" : [ "Threshold[5]", "Threshold", 0 ],
			"obj-10::obj-4::obj-108" : [ "live.text[49]", "live.text[4]", 0 ],
			"obj-10::obj-15::obj-97" : [ "live.numbox[37]", "CPU", 0 ],
			"obj-10::obj-4::obj-1::obj-15" : [ "Postamp[5]", "Postamp", 0 ],
			"obj-10::obj-10::obj-5::obj-46" : [ "live.text[52]", "live.text", 0 ],
			"obj-10::obj-4::obj-59" : [ "live.text[47]", "live.text[3]", 0 ],
			"obj-10::obj-12::obj-5::obj-45" : [ "live.text[72]", "live.text", 0 ],
			"obj-10::obj-12::obj-5::obj-27" : [ "live.text[70]", "live.text[2]", 0 ],
			"obj-10::obj-13::obj-3::obj-6" : [ "live.text[60]", "live.text", 0 ],
			"obj-10::obj-13::obj-3::obj-5" : [ "Preamp[6]", "Preamp", 0 ],
			"obj-10::obj-13::obj-24" : [ "live.dial[11]", "Delay R", 0 ]
		}
,
		"dependency_cache" : [ 			{
				"name" : "output~.model.maxpat",
				"bootpath" : "/Users/jln/@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/audio/stereo/Output/output~",
				"patcherrelativepath" : "../../../../../@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/audio/stereo/Output/output~",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "saturation~.model.maxpat",
				"bootpath" : "/Users/jln/@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/audio/stereo/Distortion/saturation~",
				"patcherrelativepath" : "../../../../../@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/audio/stereo/Distortion/saturation~",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "limiter~.model.maxpat",
				"bootpath" : "/Users/jln/@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/audio/stereo/Dynamics/limiter~",
				"patcherrelativepath" : "../../../../../@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/audio/stereo/Dynamics/limiter~",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "limiter.parametersAndMessages.maxpat",
				"bootpath" : "/Users/jln/@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/audio/stereo/Dynamics/limiter~",
				"patcherrelativepath" : "../../../../../@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/audio/stereo/Dynamics/limiter~",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "balance~.model.maxpat",
				"bootpath" : "/Users/jln/@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/audio/stereo/Imaging/balance~",
				"patcherrelativepath" : "../../../../../@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/audio/stereo/Imaging/balance~",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "record~.model.maxpat",
				"bootpath" : "/Users/jln/@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/audio/stereo/Output/record~",
				"patcherrelativepath" : "../../../../../@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/audio/stereo/Output/record~",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "record.parametersAndMessages.maxpat",
				"bootpath" : "/Users/jln/@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/audio/stereo/Output/record~",
				"patcherrelativepath" : "../../../../../@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/audio/stereo/Output/record~",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "thru.maxpat",
				"bootpath" : "/Applications/Max 6.1.8/patches/m4l-patches/Pluggo for Live resources/patches",
				"patcherrelativepath" : "../../../../../../../Applications/Max 6.1.8/patches/m4l-patches/Pluggo for Live resources/patches",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "audiotrack.maxpat",
				"bootpath" : "/Users/jln/@GMEA/i-scoreRep/Examples/Jamoma_demo/patchers",
				"patcherrelativepath" : ".",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "input~.model.maxpat",
				"bootpath" : "/Users/jln/@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/audio/stereo/Sources/input~",
				"patcherrelativepath" : "../../../../../@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/audio/stereo/Sources/input~",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "equalizer~.model.maxpat",
				"bootpath" : "/Users/jln/@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/audio/stereo/EQ/equalizer~",
				"patcherrelativepath" : "../../../../../@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/audio/stereo/EQ/equalizer~",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "j.q2octavebandwidth.maxpat",
				"bootpath" : "/Users/jln/@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/components/audio/q2octavebandwith",
				"patcherrelativepath" : "../../../../../@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/components/audio/q2octavebandwith",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "j.octavebandwidth2q.maxpat",
				"bootpath" : "/Users/jln/@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/components/audio/octavebandwidth2q",
				"patcherrelativepath" : "../../../../../@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/components/audio/octavebandwidth2q",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "echo~.model.maxpat",
				"bootpath" : "/Users/jln/@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/audio/stereo/Delay/echo~",
				"patcherrelativepath" : "../../../../../@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/audio/stereo/Delay/echo~",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "filter~.model.maxpat",
				"bootpath" : "/Users/jln/@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/audio/stereo/EQ/filter~",
				"patcherrelativepath" : "../../../../../@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/audio/stereo/EQ/filter~",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "videotrack.maxpat",
				"bootpath" : "/Users/jln/@GMEA/i-scoreRep/Examples/Jamoma_demo/patchers",
				"patcherrelativepath" : ".",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "input%.model.maxpat",
				"bootpath" : "/Users/jln/@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/video/sources/input%",
				"patcherrelativepath" : "../../../../../@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/video/sources/input%",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "noise%.model.maxpat",
				"bootpath" : "/Users/jln/@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/video/sources/noise%",
				"patcherrelativepath" : "../../../../../@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/video/sources/noise%",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "movie%.model.maxpat",
				"bootpath" : "/Users/jln/@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/video/sources/movie%",
				"patcherrelativepath" : "../../../../../@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/video/sources/movie%",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "grab%.model.maxpat",
				"bootpath" : "/Users/jln/@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/video/sources/grab%",
				"patcherrelativepath" : "../../../../../@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/video/sources/grab%",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "gl_brcosa.model.maxpat",
				"bootpath" : "/Users/jln/@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/openGL/texture/gl_brcosa",
				"patcherrelativepath" : "../../../../../@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/openGL/texture/gl_brcosa",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "j.gl_group_slab.model.maxpat",
				"bootpath" : "/Users/jln/@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/components/openGL",
				"patcherrelativepath" : "../../../../../@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/components/openGL",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "gl_videoplane.model.maxpat",
				"bootpath" : "/Users/jln/@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/openGL/geometry/gl_videoplane",
				"patcherrelativepath" : "../../../../../@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/openGL/geometry/gl_videoplane",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "j.gl_group.model.maxpat",
				"bootpath" : "/Users/jln/@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/components/openGL/gl_group",
				"patcherrelativepath" : "../../../../../@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/components/openGL/gl_group",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "gl_render.model.maxpat",
				"bootpath" : "/Users/jln/@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/openGL/gl_render",
				"patcherrelativepath" : "../../../../../@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/openGL/gl_render",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "window%.model.maxpat",
				"bootpath" : "/Users/jln/@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/video/output/window%",
				"patcherrelativepath" : "../../../../../@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/video/output/window%",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "MinuitInterface.maxpat",
				"bootpath" : "/Users/jln/@dev/@Minuit/Jamoma Interface",
				"patcherrelativepath" : "../../../../../@dev/@Minuit/Jamoma Interface",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "input~.view.maxpat",
				"bootpath" : "/Users/jln/@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/audio/stereo/Sources/input~",
				"patcherrelativepath" : "../../../../../@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/audio/stereo/Sources/input~",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "limiter~.view.maxpat",
				"bootpath" : "/Users/jln/@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/audio/stereo/Dynamics/limiter~",
				"patcherrelativepath" : "../../../../../@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/audio/stereo/Dynamics/limiter~",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "equalizer~_wide.view.maxpat",
				"bootpath" : "/Users/jln/@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/audio/stereo/EQ/equalizer~",
				"patcherrelativepath" : "../../../../../@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/audio/stereo/EQ/equalizer~",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "equalizer~_large.view.maxpat",
				"bootpath" : "/Users/jln/@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/audio/stereo/EQ/equalizer~",
				"patcherrelativepath" : "../../../../../@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/audio/stereo/EQ/equalizer~",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "echo~.view.maxpat",
				"bootpath" : "/Users/jln/@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/audio/stereo/Delay/echo~",
				"patcherrelativepath" : "../../../../../@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/audio/stereo/Delay/echo~",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "output~.view.maxpat",
				"bootpath" : "/Users/jln/@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/audio/stereo/Output/output~",
				"patcherrelativepath" : "../../../../../@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/audio/stereo/Output/output~",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "",
				"bootpath" : "/",
				"patcherrelativepath" : "../../../../../../..",
				"type" : "fold",
				"implicit" : 1
			}
, 			{
				"name" : "j.initialized.maxpat",
				"bootpath" : "/Users/jln/@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/components/data/initialized",
				"patcherrelativepath" : "../../../../../@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/components/data/initialized",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "window%.view.maxpat",
				"bootpath" : "/Users/jln/@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/video/output/window%",
				"patcherrelativepath" : "../../../../../@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/video/output/window%",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "j.list2parameter.maxpat",
				"bootpath" : "/Users/jln/@dev/@GoPlug/Projects/Orgpro-bckp/patchers",
				"patcherrelativepath" : "../../../../../@dev/@GoPlug/Projects/Orgpro-bckp/patchers",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "gl_render.view.maxpat",
				"bootpath" : "/Users/jln/@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/openGL/gl_render",
				"patcherrelativepath" : "../../../../../@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/openGL/gl_render",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "j.gl_group.panel.maxpat",
				"bootpath" : "/Users/jln/@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/components/openGL/gl_group",
				"patcherrelativepath" : "../../../../../@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/components/openGL/gl_group",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "j.gl_group.view.maxpat",
				"bootpath" : "/Users/jln/@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/components/openGL/gl_group",
				"patcherrelativepath" : "../../../../../@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/components/openGL/gl_group",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "gl_render_basic_view.maxpat",
				"bootpath" : "/Users/jln/@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/openGL/gl_render/view_tabs",
				"patcherrelativepath" : "../../../../../@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/openGL/gl_render/view_tabs",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "input%.view.maxpat",
				"bootpath" : "/Users/jln/@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/video/sources/input%",
				"patcherrelativepath" : "../../../../../@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/video/sources/input%",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "input%.ui.maxpat",
				"bootpath" : "/Users/jln/@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/video/sources/input%",
				"patcherrelativepath" : "../../../../../@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/video/sources/input%",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "gl_brcosa.view.maxpat",
				"bootpath" : "/Users/jln/@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/openGL/texture/gl_brcosa",
				"patcherrelativepath" : "../../../../../@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/openGL/texture/gl_brcosa",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "j.gl_group_slab.panel.maxpat",
				"bootpath" : "/Users/jln/@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/components/openGL",
				"patcherrelativepath" : "../../../../../@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/components/openGL",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "j.gl_group_slab.view.maxpat",
				"bootpath" : "/Users/jln/@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/components/openGL",
				"patcherrelativepath" : "../../../../../@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/components/openGL",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "gl_videoplane.view.maxpat",
				"bootpath" : "/Users/jln/@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/openGL/geometry/gl_videoplane",
				"patcherrelativepath" : "../../../../../@Jamoma/Jamoma/Implementations/Max/Jamoma/patchers/modules/openGL/geometry/gl_videoplane",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "j.xfade~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "j.model.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "j.parameter.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "j.out~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "j.message.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "j.stats.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "j.in~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "j.overdrive~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "j.limiter~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "j.panorama~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "j.return.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "j.dataspace.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "j.parameter_array.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "j.map.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "j.in.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "j.out.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "j.send.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "j.namespace.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "j.oscroute.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "j.init.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "j.receive.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "j.modular.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "j.ui.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "j.remote.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "j.view.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "j.savebang.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "j.receive~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "j.remote_array.mxo",
				"type" : "iLaX"
			}
 ]
	}

}
