/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekrause <emeric.yukii@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/13 14:32:37 by ekrause           #+#    #+#             */
/*   Updated: 2024/11/12 17:22:14 by ekrause          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	PHILO_H
# define PHILO_H

/**********/
// library

# include <stdio.h>
# include <unistd.h>
# include <stdlib.h>
# include <pthread.h>
# define RED "\033[31m"
# define END "\033[0m"

/**********/
// structures

typedef struct s_philo t_philo;
typedef struct s_fork t_fork;

typedef struct s_arg
{
	long	nb_philo;
	long	time_to_die;
	long	time_to_eat;
	long	time_to_sleep;
	long	must_eat;
	long	start;
	int		flag;
	pthread_mutex_t end_mutex;
	pthread_mutex_t write_mutex;
	t_philo	*philos;
	t_fork	*forks;
}	t_arg;

typedef struct s_fork
{
	long			id;
	pthread_mutex_t	fork;
}	t_fork;

typedef struct s_philo
{
	long		id;
	pthread_t	philo;
	long		last_meal;
	long		nbr_meals;
	t_fork		*left_fork;
	t_fork		*right_fork;
	t_arg		*arg;
}	t_philo;

#endif