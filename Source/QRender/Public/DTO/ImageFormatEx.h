// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ImageFormatEx.generated.h"

UENUM(BlueprintType)
enum class EImageFormatEx : uint8
{
	/** Invalid or unrecognized format. */
	Invalid = 0,

	/** Portable Network Graphics. */
	PNG = 1,

	/** Joint Photographic Experts Group. */
	JPEG,

	/** Single channel JPEG. */
	GrayscaleJPEG,

	/** Windows Bitmap. */
	BMP,

	/** Windows Icon resource. */
	ICO,

	/** OpenEXR (HDR) image file format. */
	EXR,

	/** Mac icon. */
	ICNS,

	/** Truevision TGA / TARGA */
	TGA,

	/** 扩展 */
	WebP
};
