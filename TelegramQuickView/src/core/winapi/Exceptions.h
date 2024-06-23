#pragma once

#define VIEWER_WARNING_BIT   0x01
#define VIEWER_ERROR_BIT     0x10

#define VIEWER_WARNING     (1 << 0)
#define VIEWER_ERROR       (1 << 4)

// Errors

// Registry 
#define VIEWER_REGISTRY_OPEN_ERROR									(VIEWER_ERROR | 0b0000)
#define VIEWER_REGISTRY_CREATE_KEY_ERROR							(VIEWER_ERROR | 0b0001)
#define VIEWER_REGISTRY_SET_VALUE_ERROR								(VIEWER_ERROR | 0b00010)

//	Errors Description

//	Registry
#define VIEWER_REGISTRY_OPEN_ERROR_DESCRIPTION						(TEXT("Не удалось открыть реестр"))
#define VIEWER_REGISTRY_CREATE_KEY_ERROR_DESCRIPTION				(TEXT("Не удалось создать ключ в реестре"))
#define VIEWER_REGISTRY_SET_VALUE_ERROR_DESCRIPTION					(TEXT("Не удалось создать пару имя-значение в реестре"))

//	Warnings

//	Registry
#define REGISTRY_ALREADY_TAKEN_AUTO_LOAD_VALUES_WARNING				(VIEWER_WARNING | 0b0000)

//	Mouse Position
#define VIEWER_CANT_GET_MOUSE_POSITION_WARNING						(VIEWER_WARNING | 0b0001)

//	Warnings Description

//	Registry
#define REGISTRY_ALREADY_TAKEN_AUTO_LOAD_VALUES_WARNING_DESCRIPTION	(TEXT("Приложение уже существует в автозагрузке"))

//	Mouse Position
#define VIEWER_CANT_GET_MOUSE_POSITION_WARNING_DESCRIPTION			(TEXT("Не удалось получить позицию мыши"))
