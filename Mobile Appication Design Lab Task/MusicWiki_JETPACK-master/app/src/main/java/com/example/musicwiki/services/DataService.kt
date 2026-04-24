package com.example.musicwiki.services

import retrofit2.Retrofit
import retrofit2.converter.gson.GsonConverterFactory

object DataService {

    private const val BASE_URL = "https://ws.audioscrobbler.com/"

    private const val API_KEY = "cb2434ffec4c3e9d4aa3de9aac98d0c1"

    val lastFmService: LastFmApi by lazy {
        Retrofit.Builder()
            .baseUrl(BASE_URL)
            .addConverterFactory(GsonConverterFactory.create())
            .build()
            .create(LastFmApi::class.java)
    }

    fun getApiKey(): String = API_KEY
}