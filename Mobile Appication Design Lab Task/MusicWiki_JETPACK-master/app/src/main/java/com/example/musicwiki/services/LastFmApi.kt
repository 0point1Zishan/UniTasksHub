package com.example.musicwiki.services

import com.example.musicwiki.model.Genre
import com.example.musicwiki.model.GenreDetail
import retrofit2.Call
import retrofit2.http.GET
import retrofit2.http.Query

interface LastFmApi {

    @GET("2.0/")
    fun getGenres(
        @Query("method") method: String,
        @Query("api_key") apiKey: String,
        @Query("format") format: String
    ): Call<Genre>

    @GET("2.0/")
    fun getTagInfo(
        @Query("method") method: String,
        @Query("tag") tagName: String,
        @Query("api_key") apiKey: String,
        @Query("format") format: String
    ): Call<GenreDetail>
}